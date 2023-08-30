#include "CSVReader.hpp"
#include "BarChart.hpp"
#include "BarChartMap.hpp"
#include "menu.hpp"


int main()
{

	struct menuData * data = menu();

	if(data->closed) {
		delete data;
		return EXIT_SUCCESS;
	}

	CSVReader csv(data->csv_data);

	BaseProject *app;

	if(data->mode == "barChartMap") {
		CSVReader csv_coordinates(data->csv_coordinates);
		app = new BarChartMap(csv, csv_coordinates, data->latitude_column, data->longitude_column, data->up, data->left, data->right, data->down, data->zoom, data->map, data->gridDim);
	} else if(data->mode == "barChart") {
		app = new BarChart(csv, data->gridDim);
	}

	delete data;

    try {
        app->run();
		delete app;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
