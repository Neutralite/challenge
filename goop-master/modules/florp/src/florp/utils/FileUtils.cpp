#include "florp/utils/FileUtils.h"
#include <fstream>
#include "Logging.h"

namespace florp {
	namespace utils {

		// Reads the entire contents of a file as a single block of data
		char* ReadFile(const char* filename) {
			// Declare and open the file stream
			std::ifstream file;
			file.open(filename, std::ios::binary);

			// Only read if the file is open
			if (file.is_open()) {
				// Get the starting location in the file
				uint64_t fileSize = file.tellg();
				// Seek to the end
				file.seekg(0, std::ios::end);
				// Calculate the file size from end to beginning
				fileSize = (uint64_t)file.tellg() - fileSize;
				// Seek back to the beginning of the file
				file.seekg(0, std::ios::beg);

				// Allocate space for our entire file, +1 byte at the end for null terminator
				char* result = new char[fileSize + 1];
				// Read the entire file to our memory
				file.read(result, fileSize);

				// Make our text null-terminated
				result[fileSize] = '\0';

				// Close the file before returning
				file.close();
				return result;
			}
			// Otherwise, we failed to open our file, throw a runtime error
			else {
				LOG_ASSERT(false, "Cannot open file: \"{}\"", filename);
			}
		}

		void* ReadFileBinary(const char* filename, size_t& outSize) {
			// Declare and open the file stream
			std::ifstream file;
			file.open(filename, std::ios::binary);

			// Only read if the file is open
			if (file.is_open()) {
				// Get the starting location in the file
				outSize = file.tellg();
				// Seek to the end
				file.seekg(0, std::ios::end);
				// Calculate the file size from end to beginning
				outSize = (uint64_t)file.tellg() - outSize;
				// Seek back to the beginning of the file
				file.seekg(0, std::ios::beg);

				// Allocate space for our entire file, +1 byte at the end for null terminator
				char* result = reinterpret_cast<char*>(malloc(outSize));
				// Read the entire file to our memory
				file.read(result, outSize);

				// Close the file before returning
				file.close();
				return result;
			}
			// Otherwise, we failed to open our file, throw a runtime error
			else {
				outSize = 0;
				LOG_ASSERT(false, "Cannot open file: \"{}\"", filename);
				return nullptr;
			}
		}
		
	}
}
