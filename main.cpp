#include <stdio.h>
#include <stdlib.h>
#include <filesystem>
#include <stdint.h>
#include <codecvt>
extern "C" {
#include <v4/eep.h>
}

static std::string getPath(const std::filesystem::path& path)
{
#ifdef WIN32
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.to_bytes(path);
#else
	return file.path();
#endif
}

static int isValidSample(const float* sample, int64_t idx, int64_t channels, const int* channelIsPercent)
{
	float limit = 1000000.0f;
	idx *= channels;
	for (int i = 0; i < channels; ++i)
	{
		float val = sample[idx + i];
		if (val > limit || val < -limit)
			return 0;
		if (channelIsPercent[i] && (val < 0 || val > 101))
			return 0;
	}
	return 1;
}

static void process_file(const std::filesystem::directory_entry& inputFile)
{
	std::string inputFilePath = getPath(inputFile.path());
	printf("Process file: %s\n", inputFilePath.c_str());

	cntfile_t handle = libeep_read_with_external_triggers(inputFilePath.c_str());
	if (handle == -1) {
		printf("  ERROR: could not read the file\n");
		return;
	}
	int sampfreq = libeep_get_sample_frequency(handle);
	int chanc = libeep_get_channel_count(handle);
	int64_t sampc = libeep_get_sample_count(handle);
	printf("  %i channels, %lli samples at %i Hz = %.1f sec\n", chanc, sampc, sampfreq, double(sampc)/double(sampfreq));

	std::filesystem::path outputFile = inputFile.path();
	outputFile.replace_extension(".csv");
	std::string outputFilePath = getPath(outputFile);

	FILE* fout = fopen(outputFilePath.c_str(), "wt");
	if (fout == NULL)
	{
		printf("  ERROR: could write output file '%s'\n", outputFilePath.c_str());
		libeep_close(handle);
		return;
	}

	// output channel names/types
	int* channelIsPercent = new int[chanc];
	for (int i = 0; i < chanc; ++i)
	{
		const char* clabel = libeep_get_channel_label(handle, i);
		const char* cunit = libeep_get_channel_unit(handle, i);
		fprintf(fout, "%s %s, ", clabel, cunit);
		channelIsPercent[i] = (0 == strcmp(cunit, "%"));
	}
	fprintf(fout, "\n");

	// get samples
	float* sample = libeep_get_samples(handle, 0, long(sampc));

	// skip invalid samples from start
	int skippedStart = 0, skippedEnd = 0;
	int64_t sampleIdx = 0;
	while (sampleIdx < sampc && !isValidSample(sample, sampleIdx, chanc, channelIsPercent))
	{
		++sampleIdx;
		++skippedStart;
	}
	// skip invalid samples from end
	while (sampc > sampleIdx + 1 && !isValidSample(sample, sampc - 1, chanc, channelIsPercent))
	{
		--sampc;
		++skippedEnd;
	}

	// average sample values for each minute
	int linesPrinted = 0;
	int samplesToAverage = sampfreq * 60;
	float* avgValues = new float[chanc];
	while (sampleIdx < sampc)
	{
		for (int i = 0; i < chanc; ++i)
			avgValues[i] = 0;

		int toAvg = samplesToAverage;
		if (sampleIdx + toAvg > sampc)
			toAvg = int(sampc - sampleIdx);
		if (toAvg < 1)
			break;
		for (int s = 0; s < toAvg; ++s)
		{
			for (int c = 0; c < chanc; ++c)
				avgValues[c] += sample[sampleIdx * chanc + c];
			++sampleIdx;
		}
		for (int i = 0; i < chanc; ++i)
		{
			fprintf(fout, "%f, ", avgValues[i] / toAvg);
		}
		fprintf(fout, "\n");
		++linesPrinted;
	}
	delete[] avgValues;
	delete[] channelIsPercent;
	libeep_free_samples(sample);
	fclose(fout);

	printf("  wrote '%s': %i entries, ignored %.2fs at start and %.2fs at end\n", outputFilePath.c_str(), linesPrinted, double(skippedStart)/sampfreq, double(skippedEnd)/sampfreq);
	libeep_close(handle);
}


int main(int argc, char** argv)
{
	printf("Processing all CNT files...\n");
	libeep_init();

	for (const auto& entry : std::filesystem::directory_iterator("."))
	{
		if (entry.path().extension() == ".cnt")
		{
			process_file(entry);
		}
	}

	libeep_exit();
	printf("Done! Press Enter.\n");
	getc(stdin);
	return 0;
}
