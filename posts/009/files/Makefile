capture_in_lambda: capture_in_lambda.cpp
	mkdir -p build/
	# c++ -O3 -Wall -Wextra -std=c++17 -o build/capture_in_lambda capture_in_lambda.cpp -lpthread
	c++ -O3 -Wall -Wextra -std=c++17 -o build/capture_in_lambda capture_in_lambda.cpp -lpthread -fsanitize=address
clean:
	rm -f build/capture_in_lambda