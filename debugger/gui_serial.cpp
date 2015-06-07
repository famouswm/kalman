// g++ ./gui_serial -o gui_serial `sdl2-config --cflags --libs`

#include <iostream>
#include <SDL.h>
#include "serial.h"
#include "serial.cpp"

SDL_Renderer *global_renderer;

void set_color(uint32 color)
{
	SDL_SetRenderDrawColor(global_renderer, (uint8)(color >> 24), (uint8)(color >> 16), (uint8)(color >> 8), (uint8)(color));
}

void clear(uint32 color)
{
	set_color(color);
	SDL_RenderClear(global_renderer);
}

void draw_point(int x, int y)
{
	SDL_RenderDrawPoint(global_renderer, x, y);
}

void draw_line(int x1, int y1, int x2, int y2)
{
	SDL_RenderDrawLine(global_renderer, x1, y1, x2, y2);
}

struct DataPoint
{
	float value;
	DataPoint *next;
};

struct DataSeries
{
	DataPoint *first;
	DataPoint *last;
	float max_value;
	float min_value;
	int max_data_count;
};

void clear_data_series(DataSeries *series)
{
	DataPoint *dp = series->first;
	while (dp)
	{
		DataPoint *temp = dp;
		dp = dp->next;
		delete temp;
	}
	series->first = 0;
	series->last = 0;
	series->max_data_count = 0;
	series->max_value = 1.0f;
	series->min_value = 0.0f;
}

DataSeries new_data_series(int max_data_count)
{
	DataSeries result;
	DataPoint *dp = new DataPoint();
	result.first = dp;
	dp->value = 0.0f;
	for (int i = 0; i < max_data_count; i++)
	{
		DataPoint *next = new DataPoint();
		next->value = 0.0f;
		next->next = 0;
		dp->next = next;
		dp = next;
	}
	result.last = dp;
	result.max_value = 1.0f;
	result.min_value = 0.0f;
	result.max_data_count = max_data_count;
	return result;
}

float min(float x, float y) { return x < y ? x : y; }
float max(float x, float y) { return x > y ? x : y; }

void add_data_point(DataSeries *series, float y)
{
	DataPoint *temp = series->first;
	series->first = series->first->next;
	delete temp;
	DataPoint *new_point = new DataPoint();
	new_point->value = y;
	new_point->next = 0;
	series->last->next = new_point;
	series->last = new_point;

	series->max_value = max(series->max_value, y);
	series->min_value = min(series->min_value, y);
}

void draw_data_series(DataSeries *series,
                      float rect_x_nc, float rect_y_nc,
                      float rect_width_nc, float rect_height_nc,
                      int window_width, int window_height,
                      uint32 color)
{
	int rect_x = rect_x_nc * window_width;
	int rect_y = rect_y_nc * window_height;
	int rect_width = rect_width_nc * window_width;
	int rect_height = rect_height_nc * window_height;
	set_color(color);
	float dx = rect_width / (float)series->max_data_count;
	DataPoint *p = series->first;
	float x0 = 0.0f;
	float y0 = rect_height * (p->value - series->min_value) / (series->max_value - series->min_value);
	int i = 0;
	while (p)
	{
		float x = i * dx;
		float y = rect_height * (p->value - series->min_value) / (series->max_value - series->min_value);
		draw_line(rect_x + (int)x0, rect_y + (int)y0, rect_x + (int)x, rect_y + (int)y);
		x0 = x;
		y0 = y;
		i++;
		p = p->next;
	}
}

struct SensorPacket
{
    union
    {
        uint8 raw_bytes[18];
        struct
        {
        	int16 mag_x, mag_y, mag_z;
            int16 gyro_x, gyro_y, gyro_z;
            int16 accel_x, accel_y, accel_z;
        };
    };
};

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("Failed to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *window = NULL;
	uint32 window_flags = SDL_WINDOW_SHOWN;
	int window_width = 1000;
	int window_height = 600;
	window = SDL_CreateWindow("Sensor GUI", 100, 100, window_width, window_height, window_flags);
	if (!window)
	{
		printf("Failed to create window: %s\n", SDL_GetError());
		return 1;
	}

	global_renderer = NULL;
	global_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!global_renderer)
	{
		printf("Failed to create renderer: %s\n", SDL_GetError());
		return 1;
	}

	assert(argc == 3);
    const char *port_name = argv[1]; // mbed shows up as ttyACM0 on my machine
    int baud_rate = atoi(argv[2]); // 9600 is usually good, they say?
    Serial port = serial_open(port_name, baud_rate);
    if (!port.is_valid)
    {
    	printf("Failed to open serial port");
    	return 1;
    }
    actual_serial_flush(&port);

    // Synchronize
    uint8 handshake = 0xab;
    serial_write(&port, &handshake, 1);

    DataSeries series_mag_x = new_data_series(200);
    DataSeries series_mag_y = new_data_series(200);
    DataSeries series_mag_z = new_data_series(200);

    DataSeries series_gyro_x = new_data_series(200);
    DataSeries series_gyro_y = new_data_series(200);
    DataSeries series_gyro_z = new_data_series(200);

    DataSeries series_accel_x = new_data_series(200);
    DataSeries series_accel_y = new_data_series(200);
    DataSeries series_accel_z = new_data_series(200);

	bool game_running = true;
	while (game_running)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
				{
					game_running = false;
				} break;
				case SDL_KEYUP:
				{
					if (event.key.keysym.sym == SDLK_r)
					{
						series_mag_x.min_value = 0.0f; series_mag_x.max_value = 1.0f;
						series_mag_y.min_value = 0.0f; series_mag_y.max_value = 1.0f;
						series_mag_z.min_value = 0.0f; series_mag_z.max_value = 1.0f;

						series_gyro_x.min_value = 0.0f; series_gyro_x.max_value = 1.0f;
						series_gyro_y.min_value = 0.0f; series_gyro_y.max_value = 1.0f;
						series_gyro_z.min_value = 0.0f; series_gyro_z.max_value = 1.0f;

						series_accel_x.min_value = 0.0f; series_accel_x.max_value = 1.0f;
						series_accel_y.min_value = 0.0f; series_accel_y.max_value = 1.0f;
						series_accel_z.min_value = 0.0f; series_accel_z.max_value = 1.0f;
					}
				} break;
			}
		}

		int buffer_length = sizeof(SensorPacket);
        uint8 *buffer = new uint8[buffer_length];
        for (int i = 0; i < buffer_length; i++)
            buffer[i] = 0;

        while (serial_bytes_available(&port) >= sizeof(SensorPacket))
        {
            int bytes_read = serial_read(&port, buffer, sizeof(SensorPacket));
            SensorPacket *read_packet = (SensorPacket*)buffer;
            add_data_point(&series_mag_x, read_packet->mag_x);
            add_data_point(&series_mag_y, read_packet->mag_y);
            add_data_point(&series_mag_z, read_packet->mag_z);
            add_data_point(&series_gyro_x, read_packet->gyro_x);
            add_data_point(&series_gyro_y, read_packet->gyro_y);
            add_data_point(&series_gyro_z, read_packet->gyro_z);
            add_data_point(&series_accel_x, read_packet->accel_x);
            add_data_point(&series_accel_y, read_packet->accel_y);
            add_data_point(&series_accel_z, read_packet->accel_z);
        }

        uint8 handshake = 0xab;
        serial_write(&port, &handshake, 1);

		clear(0x282222ff);
		draw_data_series(&series_mag_x, 0.0f, 0.0f, 0.3f, 0.3f, window_width, window_height, 0xff5533ff);
		draw_data_series(&series_mag_y, 0.33f, 0.0f, 0.3f, 0.3f, window_width, window_height, 0x55ff33ff);
		draw_data_series(&series_mag_z, 0.66f, 0.0f, 0.3f, 0.3f, window_width, window_height, 0x3355ffff);
		draw_data_series(&series_gyro_x, 0.0f, 0.33f, 0.3f, 0.3f, window_width, window_height, 0xff5533ff);
		draw_data_series(&series_gyro_y, 0.33f, 0.33f, 0.3f, 0.3f, window_width, window_height, 0x55ff33ff);
		draw_data_series(&series_gyro_z, 0.66f, 0.33f, 0.3f, 0.3f, window_width, window_height, 0x3355ffff);
		draw_data_series(&series_accel_x, 0.0f, 0.66f, 0.3f, 0.3f, window_width, window_height, 0xff5533ff);
		draw_data_series(&series_accel_y, 0.33f, 0.66f, 0.3f, 0.3f, window_width, window_height, 0x55ff33ff);
		draw_data_series(&series_accel_z, 0.66f, 0.66f, 0.3f, 0.3f, window_width, window_height, 0x3355ffff);

		SDL_RenderPresent(global_renderer);
		SDL_Delay(16);
	}

	SDL_DestroyRenderer(global_renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
    serial_close(&port);

	return EXIT_SUCCESS;
}
