#include "CSVReader.hpp"
#include "BarChartMap.hpp"
// #include "BarChartMap.hpp"
extern "C" {
	#include "mercator.h"
}

// This is the main: probably you do not need to touch this!
int main() {
    CSVReader csv("data/cases_by_region.csv");
	CSVReader csv_coordinates("data/region_coordinates.csv");
	
	bar_coordinates = new coordinates[csv_coordinates.getNumLines()];
	printf("up: %f, sx: %f, dx: %f, down: %f\n", up_coordinates, sx_coordinates, dx_coordinates, down_coordinates);
	for (int i = 0; i < csv_coordinates.getNumLines(); i++) {
		// Converting latitude and longitude to mercator Cartesian coordinates	
		bar_coordinates[i].x = degreeLatitudeToY(std::stof(csv_coordinates.getLine(i)[2]));
		bar_coordinates[i].z = degreeLongitudeToX(std::stof(csv_coordinates.getLine(i)[3]));
		// Scaling and translating the coordinates
		bar_coordinates[i].z = -zoom * (bar_coordinates[i].z - sx_coordinates - (dx_coordinates - sx_coordinates) / 2.f);
		bar_coordinates[i].x = zoom * (up_coordinates - bar_coordinates[i].x - (up_coordinates - down_coordinates) / 2.f);

		// printf("%d: x: %f, z: %f;\t%s\n", i, bar_coordinates[i].x, bar_coordinates[i].z, csv_coordinates.getLine(i)[1].c_str());
	}

	// getchar();

    BarChartMap app(csv);

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
