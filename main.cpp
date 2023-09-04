#include "CSVReader.hpp"
#include "BarChart.hpp"
#include "BarChartMap.hpp"
#include "menu.hpp"


int main(int argc, char* argv[])
{

	struct menuData * data = menu();

	if(data->closed) {
		delete data;
		return EXIT_SUCCESS;
	}

	CSVReader csv(data->csv_data);


	BaseProject *app;
	std::string executablePath = argv[0];
	std::string executableDir = executablePath.substr(0, executablePath.find_last_of("\\/"));
	std::string shaderDir = executableDir + "/shaders/";

	if(data->mode == "barChartMap") {
		CSVReader csv_coordinates(data->csv_coordinates);
		app = new BarChartMap(data->title, shaderDir, csv, csv_coordinates, data->latitude_column, data->longitude_column, data->up, data->left, data->right, data->down, data->zoom, data->map, data->gridDim);
	} else if(data->mode == "barChart") {
		app = new BarChart(data->title, shaderDir, csv, data->gridDim);
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
