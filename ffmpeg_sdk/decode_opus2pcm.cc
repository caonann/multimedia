#include "ffmpeg_sdk/decode_opus2pcm.h"

static int get_format_from_sample_fmt(const char **fmt, enum AVSampleFormat sample_fmt) {
  int i;
  struct sample_fmt_entry {
    enum AVSampleFormat sample_fmt;
    const char *fmt_be, *fmt_le;
  } sample_fmt_entries[] = {
      {AV_SAMPLE_FMT_U8, "u8", "u8"},        {AV_SAMPLE_FMT_S16, "s16be", "s16le"},
      {AV_SAMPLE_FMT_S32, "s32be", "s32le"}, {AV_SAMPLE_FMT_FLT, "f32be", "f32le"},
      {AV_SAMPLE_FMT_DBL, "f64be", "f64le"},
  };
  *fmt = NULL;

  for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) {
    struct sample_fmt_entry *entry = &sample_fmt_entries[i];
    if (sample_fmt == entry->sample_fmt) {
      *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
      return 0;
    }
  }

  fprintf(stderr, "sample format %s is not supported as output format\n", av_get_sample_fmt_name(sample_fmt));
  return -1;
}

static void decode(AVCodecContext *dec_ctx, AVPacket *pkt, AVFrame *frame, FILE *outfile) {
  int i;
  int ch;
  int ret;
  int data_size;

  /* send the packet with the compressed data to the decoder */
  ret = avcodec_send_packet(dec_ctx, pkt);
  if (ret < 0) {
    fprintf(stderr, "Error submitting the packet to the decoder\n");
    exit(1);
  }

  /* read all the output frames (in general there may be any number of them */
  while (ret >= 0) {
    ret = avcodec_receive_frame(dec_ctx, frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
      return;
    }
    if (ret < 0) {
      fprintf(stderr, "Error during decoding\n");
      exit(1);
    }
    data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
    if (data_size < 0) {
      /* This should not occur, checking just for paranoia */
      fprintf(stderr, "Failed to calculate data size\n");
      exit(1);
    }
    for (i = 0; i < frame->nb_samples; i++)
      for (ch = 0; ch < dec_ctx->channels; ch++) fwrite(frame->data[ch] + data_size * i, 1, data_size, outfile);
  }
}

int main(int argc, char *argv[]) {
  std::cout << argv[0] << " Version " << VERSION_MAJOR << "." << VERSION_MINOR << std::endl;
  const char *outfilename, *filename;
  const AVCodec *codec;
  AVCodecContext *c = nullptr;
  AVCodecParserContext *parser = nullptr;
  int len, ret;
  FILE *f, *outfile;
  uint8_t inbuf[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
  uint8_t *data;
  size_t data_size;
  AVPacket *pkt;
  AVFrame *decoded_frame = nullptr;
  enum AVSampleFormat sfmt;
  int n_channels = 0;
  const char *fmt;

  if (argc <= 2) {
    fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
    exit(0);
  }
  filename = argv[1];
  outfilename = argv[2];

  pkt = av_packet_alloc();

  /* find the MPEG audio decoder */
  codec = avcodec_find_decoder(AV_CODEC_ID_OPUS);
  if (codec == nullptr) {
    fprintf(stderr, "Codec not found\n");
    exit(1);
  }

  parser = av_parser_init(codec->id);
  if (parser == nullptr) {
    fprintf(stderr, "Parser not found\n");
    exit(1);
  }

  c = avcodec_alloc_context3(codec);
  if (c == nullptr) {
    fprintf(stderr, "Could not allocate audio codec context\n");
    exit(1);
  }

  /* open it */
  if (avcodec_open2(c, codec, nullptr) < 0) {
    fprintf(stderr, "Could not open codec\n");
    exit(1);
  }

  f = fopen(filename, "rb");
  if (f == nullptr) {
    fprintf(stderr, "Could not open %s\n", filename);
    exit(1);
  }
  outfile = fopen(outfilename, "wb");
  if (outfile == nullptr) {
    av_free(c);
    exit(1);
  }

  /* decode until eof */
  data = inbuf;
  data_size = fread(inbuf, 1, AUDIO_INBUF_SIZE, f);

  while (data_size > 0) {
    if (decoded_frame == nullptr) {
      if (!(decoded_frame = av_frame_alloc())) {
        fprintf(stderr, "Could not allocate audio frame\n");
        exit(1);
      }
    }

    ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size, data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
    if (ret < 0) {
      fprintf(stderr, "Error while parsing\n");
      exit(1);
    }
    data += ret;
    data_size -= ret;

    if (pkt->size != 0) {
      decode(c, pkt, decoded_frame, outfile);
    }

    if (data_size < AUDIO_REFILL_THRESH) {
      memmove(inbuf, data, data_size);
      data = inbuf;
      len = fread(data + data_size, 1, AUDIO_INBUF_SIZE - data_size, f);
      if (len > 0) {
        data_size += len;
      }
    }
  }

  /* flush the decoder */
  pkt->data = nullptr;
  pkt->size = 0;
  decode(c, pkt, decoded_frame, outfile);

  /* print output pcm infomations, because there have no metadata of pcm */
  sfmt = c->sample_fmt;

  if (av_sample_fmt_is_planar(sfmt) != 0) {
    const char *packed = av_get_sample_fmt_name(sfmt);
    printf(
        "Warning: the sample format the decoder produced is planar "
        "(%s). This example will output the first channel only.\n",
        packed != nullptr ? packed : "?");
    sfmt = av_get_packed_sample_fmt(sfmt);
  }

  n_channels = c->channels;
  if ((ret = get_format_from_sample_fmt(&fmt, sfmt)) < 0) {
    goto end;
  }

  printf(
      "Play the output audio file with the command:\n"
      "ffplay -f %s -ac %d -ar %d %s\n",
      fmt, n_channels, c->sample_rate, outfilename);
end:
  fclose(outfile);
  fclose(f);

  avcodec_free_context(&c);
  av_parser_close(parser);
  av_frame_free(&decoded_frame);
  av_packet_free(&pkt);

  return 0;
}