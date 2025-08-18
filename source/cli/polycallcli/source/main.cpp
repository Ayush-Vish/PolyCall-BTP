
#include <metacallcli/application.hpp>

/* -- Namespace Declarations -- */

using namespace metacallcli;

/* -- Methods -- */

int main(int argc, char *argv[])
{
	application app(argc, argv);

	app.run();

	return 0;
}
