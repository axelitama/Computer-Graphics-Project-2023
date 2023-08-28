#include "CSVReader.hpp"
#include "BarChart.hpp"
#include "BarChartMap.hpp"

int main() {

	/// TODO: make a menu

	CSVReader csv("data/cases_by_region.csv");
	BaseProject *app;

	#define MAP true
	if(MAP) {
		CSVReader csv_coordinates("data/region_coordinates.csv");
		app = new BarChartMap(csv, csv_coordinates, 47.5f, 5.f, 20.f, 34.5f, 0.00002);
	} else {
		app = new BarChart(csv);
	}
    

    try {
        app->run();
		delete app;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
