#pragma once

namespace florp {
	namespace utils {

		/*
		 * Reads the entire contents of the given file as a single text blob (null-terminated)
		 * @param fileName The path to the file to read
		 * @returns The null-terminated text loaded from the file, allocated with new
		 */
		char* ReadFile(const char* filename);

		/*
		 * Reads the entire contents of the given file as a single binary blob
		 * @param fileName The path to the file to read
		 * @param outSize A reference to a value to store the size of the binary blob
		 * @returns The binary blob loaded from the file, allocated with malloc
		 */
		void* ReadFileBinary(const char* filename, size_t& outSize);
	}
}