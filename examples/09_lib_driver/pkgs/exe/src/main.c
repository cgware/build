#include "driver.h"
#include "example.h"

int main()
{
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != 1) {
			continue;
		}
		example_t *drv = i->data;
		drv->f();
	}
	return 0;
}
