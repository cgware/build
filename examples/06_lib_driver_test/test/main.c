#include "driver.h"
#include "drv/drv.h"
#include "drv/drv_priv.h"
#include "lib/lib.h"
#include "lib/lib_priv.h"

int main()
{
	int ret = drv() + drv_priv() + lib() + lib_priv();
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != 1) {
			continue;
		}
		drv_t *drv = i->data;
		ret += (*drv)();
	}
	return ret;
}
