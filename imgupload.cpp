// everything is in the _buffer variable.

#include "Includes/WebServer.hpp"

void splitString(const std::string &input, std::vector<std::string> &output, const char *delimiter) {
	char *token = strtok(const_cast<char *>(input.c_str()), delimiter);
	while (token != nullptr) {
		output.push_back(token);
		token = strtok(nullptr, delimiter);
	}
}

int main(void)
{
	// in the for loops, i is an iterator that point to the string in the tab vector. *i gives us the line. 
	std::string boundary;
	// the buffer will be replaced by the string containing the request already usable like the example bellow, in the handler::parse() function.
	std::string buffer = "POST /upload_images HTTP/1.1\r\nHost: example.com\r\nContent-Type: multipart/form-data; boundary=---------------------------1234567890\r\n\r\n-----------------------------1234567890\r\nContent-Disposition: form-data: name=\"image1\"; filename=\"image1.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n[Binary data for image1.jpg goes here]\r\n-----------------------------1234567890\r\nContent-Disposition: form-data: name=\"image2\"; filename=\"image2.png\"\r\nContent-Type: image/png\r\n\r\n[Binary data for image2.png goes here]\r\n-----------------------------1234567890--\r\n";
	std::cout << "===================== Recieved Request =====================\n" << std::endl;
	// std::cout << buffer << "\n\n" << std::endl;
	std::vector<std::string> tab; // these are all the strings once the request is parsed using "\r\n".
	std::vector<std::vector<std::string> > blocks; // each block is a group of lines composing a part of the body.
	std::vector<std::string> currentBlock;


	// SEPERATE THE REQUEST INTO LINES USING "\r\n"

	std::cout << "===================== Entering Parsing =====================\n" << std::endl;
	splitString(buffer, tab, "\r\n");
	// printing the parsed lines one by one, juste for tests.
	// for (std::vector<std::string>::iterator i = tab.begin(); i != tab.end(); ++i) {
	// 	std::cout << "===================== New line =====================\n" << std::endl;
	// 	std::cout << *i << "\n\n" << std::endl;
	// }
	

	// LOOKING FOR THE BOUNDARY

	// iterating on every line of the request, iterator started at the beginning of the vector's 'list' and keeps going as long as it is not the end of the vector.
	// it might be better to stop iterating before the end of the request, since the boundary needs to be specified in the header, once we exit the header ("\r\n\r\n")
	// we already know there is a problem
	for (std::vector<std::string>::iterator i = tab.begin(); i != tab.end(); ++i) {
		// looking for the "boundary=" string inside the lines of the request. It might not always be on the same line number.
		if (strstr(i->c_str(), "boundary=")) {
			// we enter here if we find the string in the string.
			std::cout << *i << std::endl; // only for testing
			for (std::vector<std::string>::iterator i = tab.begin(); i != tab.end(); ++i) {
    			size_t found = i->find("boundary=");
				//npos means "not found" or "end of line" it is returned by find if the string is not found.
    			if (found != std::string::npos) {
        			// Extract the substring after "boundary="
        			boundary = i->substr(found + 9); // +9 to skip "boundary="
        			break; // Stop searching after finding the first occurrence
    			}
			}
			// Now 'boundary' contains the characters that come after "boundary="
			if (!boundary.empty()) {
    		// You can use 'boundary' somewhere else, juste for testing purpose.
    		std::cout << "Boundary: " << boundary << "\n\n" << std::endl;
			}
			else {
				std::cout << "No 'boundary=' found in the request" << std::endl;
			}
		}
	}


	// REGROUPING INTO BLOCKS (std::vector<std::vector<std::string> > blocks)

	//iteration on each string
	for (std::vector<std::string>::iterator i = tab.begin(); i != tab.end(); ++i) {
		if (i->find(boundary) != std::string::npos) {
			// we found a boundary so we add first the current block to the blocks vector.
			blocks.push_back(currentBlock);
			currentBlock.clear(); // it does not clear the newly add block in blocks, only the currentBlock, which after the push_back are two different things.
		}
		else {
			// the line is not a boundary, we can add the line to the current block.
			currentBlock.push_back(*i);
		}
	}
	// once we exit the loop we still have the last block to add the blocks.
	// we just need to make sure it is not empty. I am not sure we need that if the last line of a multipart request is a boundary, we will never enter this condition.
	if (!currentBlock.empty()) {
		std::cout << "last line not a boundary, good to know" << std::endl;
		blocks.push_back(currentBlock);
	}
	// printing the blocks just for tests.
	std::cout << "===================== BLOCKS =====================\n\n\n" << std::endl;
	for (size_t i = 0; i < blocks.size(); ++i) {
		std::cout << "Block " << i + 1 << ":" << std::endl;
		for (size_t j = 0; j < blocks[i].size(); ++j) {
			std::cout << blocks[i][j] << std::endl;
		}
		std::cout << "\n" << std::endl;
	}
	return 0;
}

//add to obsidian : 
//https://www.tutorialspoint.com/http/http_requests.htm
//https://cplusplus.com/reference/cstring/
