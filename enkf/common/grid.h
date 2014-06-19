/******************************************************************************
 *
 * File:        grid.h        
 *
 * Created:     12/2012
 *
 * Author:      Pavel Sakov
 *              Bureau of Meteorology
 *
 * Description:
 *
 * Revisions:
 *
 *****************************************************************************/

#if !defined(_GRID_H)

#define NGRIDTYPE 3
#define GRIDTYPE_NONE 0
#define GRIDTYPE_LATLON_REGULAR 1
#define GRIDTYPE_LATLON_IRREGULAR 2
#define GRIDTYPE_CURVILINEAR 3

typedef void (*grid_ll2fij_fn) (void* grid, double x, double y, double* fi, double* fj);
typedef void (*grid_z2fk_fn) (void* grid, double fi, double fj, double z, double* fk);
typedef void (*grid_fij2ll_fn) (void* grid, double fi, double fj, double* lon, double* lat);

struct grid;
typedef struct grid grid;

grid* grid_create(void);
void grid_destroy(grid* g);
void grid_getdimensions(grid* g, int* ni, int* nj, int* nk);
void grid_set(grid* g, int type, int periodic_x, int periodic_y, int nx, int ny, int nz, void* x, void* y, double* z);
int grid_gettype(grid* g);
grid_ll2fij_fn grid_getll2fijfn(grid* g);
grid_z2fk_fn grid_getz2fkfn(grid* g);
grid_fij2ll_fn grid_getfij2llfn(grid* g);
int grid_isperiodic_x(grid* g);
int grid_isperiodic_y(grid* g);
int grid_getlontype(grid* g);

#define _GRID_H
#endif