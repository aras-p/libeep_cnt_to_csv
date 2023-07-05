// system
#include <stdio.h>
#include <stdlib.h>
// libeep
#include <v4/eep.h>
///////////////////////////////////////////////////////////////////////////////

static int isValidSample(const float* sample, long idx, long channels)
{
    float limit = 1000000.0f;
    float limitLast3 = 101.0f;
    idx *= channels;
    for (int i = 0; i < channels; ++i)
    {
      float val = sample[idx+i];
      if (val > limit || val < -limit)
        return 0;
      if (i >= channels-3) {
        if (val > limitLast3)
          return 0;
      }
    }
    return 1;
}

void
handle_file(const char *filename) {
  int i, handle, c, chanc, triggerc;
  long s, sampc, sampfreq;

  handle = libeep_read_with_external_triggers(filename);
  if(handle == -1) {
    fprintf(stderr, "error opening %s", filename);
  }

  // frequency
  sampfreq = libeep_get_sample_frequency(handle);

  // channels
  chanc = libeep_get_channel_count(handle);
  //printf("channels: %i\n", chanc);
  for (i = 0; i < chanc; ++i) {
    const char* clabel = libeep_get_channel_label(handle, i);
    const char* cunit = libeep_get_channel_unit(handle, i);
    printf("%s %s, ", clabel, cunit);
  }
  printf("\n");

  // samples
  sampc = libeep_get_sample_count(handle);
  float * sample = libeep_get_samples(handle, 0, sampc);

  // skip invalid samples from start
  long sampleIdx = 0;
  while (sampleIdx < sampc && !isValidSample(sample, sampleIdx, chanc))
    ++sampleIdx;
  // skip invalid samples from end
  //long sampcOrig = sampc;
  while (sampc > sampleIdx + 1 && !isValidSample(sample, sampc-1, chanc))
    --sampc;
  //printf("# skipped %i start, %i end\n", (int)sampleIdx, (int)(sampcOrig-sampc));

  // average sample values for each minute
  long samplesToAverage = sampfreq * 60;
  float* avgValues = (float*)malloc(chanc * sizeof(float));
  while (sampleIdx < sampc)
  {
    for (i = 0; i < chanc; ++i)
      avgValues[i] = 0;
    long toAvg = samplesToAverage;
    if (sampleIdx + toAvg > sampc)
      toAvg = sampc - sampleIdx;
    if (toAvg < 1)
      break;
    for (s = 0; s < toAvg; ++s)
    {
      for (c = 0; c < chanc; ++c)
        avgValues[c] += sample[sampleIdx * chanc + c];
      ++sampleIdx;
    }
    for (i = 0; i < chanc; ++i)
    {
      printf("%f, ", avgValues[i] / toAvg);
    }
    printf("\n");
  }
  libeep_free_samples(sample);
  free(avgValues);

  // triggers
  /*
  triggerc = libeep_get_trigger_count(handle); 
  printf("triggers: %i\n", triggerc);
  for(i=0;i<triggerc;++i) {
    const char * code;
    uint64_t     offset;
    code = libeep_get_trigger(handle, i, & offset);
    printf("trigger(%i, %s, %lu)\n", i, code, offset);
  }
  */

  // close
  libeep_close(handle);
}
///////////////////////////////////////////////////////////////////////////////
int
main(int argc, char **argv) {
  libeep_init();

  int i;
  for(i=1;i<argc;i++) {
    handle_file(argv[i]);
  }

  libeep_exit();
  return 0;
}
