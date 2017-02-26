CarSoccer: main.cpp draw.hpp engine.hpp
	g++ main.cpp -o CarSoccer `pkg-config --cflags --libs sdl2 glew` -I./glm-0.9.8.4

clean:
	rm CarSoccer
