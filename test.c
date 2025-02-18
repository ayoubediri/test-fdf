/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiri <adiri@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 17:04:36 by adiri             #+#    #+#             */
/*   Updated: 2025/02/18 16:00:33 by adiri            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <stdio.h>
#include "fdf.h"




typedef struct {
    double x2D;
    double y2D;
	double z2D;
} Point2D;

typedef struct s_d {
    void *mlx;
    void *win;
    int x; // X-coordinate of a moving square
} t_d;

typedef struct s1
{
    int x0;
    int x1;
    int y0;
    int y1;
	char *data;
	int	bpp;
	int s_l;
	int endian;
	void	*mlx;
	void	*win;
	void	*img;
	int		z;
	double		d;
	double		rot_x;
	double		rot_y;
	Point2D		**p;
} t_s1;

void isometric_project(int x, int y, int z, int *x_proj, int *y_proj) {
    *x_proj = (x - y) / sqrt(2);
    *y_proj = (x + 2 * z + y) / sqrt(6);
}

/*
void set_pixel(char *data, int x, int y, int color, int bpp, int size_line)
{
	int pixel_index;
	pixel_index = (y * size_line) + (x * (bpp / 8));
	*(unsigned int *)(data + pixel_index) = color;
}
*/
void draw_line(char *data, int x0, int y0, int x1, int y1, int color, int bpp, int size_line)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (x0 != x1 || y0 != y1)
    {
		int pixel_index;
    	pixel_index = (y0 * size_line) + (x0 * (bpp / 8));
        *(unsigned int *)(data + pixel_index) = color;
        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}


int update(void *param)
{
    t_d *data = (t_d *)param;

    mlx_clear_window(data->mlx, data->win);

    mlx_string_put(data->mlx, data->win, data->x, 100, 0xFFFFFF, "Moving Square");

    data->x += 5;
    if (data->x > 800) // Reset when it goes off-screen
        data->x = 0;
    usleep(50000); // Slow down the loop (50ms)
    return 0;
}

void rotate_x(Point2D *point, double angle) {
    double temp_y = point->y2D;
    point->y2D = point->y2D * cos(angle) - point->z2D * sin(angle);
    point->z2D = temp_y * sin(angle) + point->z2D * cos(angle);
}

void rotate_y(Point2D *point, double angle) {
    double temp_x = point->x2D;
    point->x2D = point->x2D * cos(angle) + point->z2D * sin(angle);
    point->z2D = -temp_x * sin(angle) + point->z2D * cos(angle);
}

void apply_rotation(Point2D *point, double angle_x, double angle_y) {
    rotate_x(point, angle_x);
    rotate_y(point, angle_y);
}

int test(int key, t_s1 *data) {
    if (key == 'x') // Rotate around X-axis
        data->rot_x += 0.05;
    if (key == 'c') // Rotate around X-axis in the opposite direction
        data->rot_x -= 0.05;
    if (key == 'y') // Rotate around Y-axis
        data->rot_y += 0.05;
    if (key == 'z') // Rotate around Y-axis in the opposite direction
        data->rot_y -= 0.05;
	if (key == 'l')
		data->d += 0.05;
	if (key == 'k')
        data->d -= 0.05;

    int rows = 10, cols = 10;
    Point2D **p = (Point2D **)malloc(rows * sizeof(Point2D *));
    if (p == NULL) {
        perror("Failed to allocate memory for rows");
        return 1;
    }
    for (int i = 0; i < rows; i++) {
        p[i] = (Point2D *)malloc(cols * sizeof(Point2D));
        if (p[i] == NULL) {
            perror("Failed to allocate memory for a row");
            for (int j = 0; j < i; j++) {
                free(p[j]);
            }
            free(p);
            return 1;
        }
    }

    // Angle for rotation
    double angle = M_PI / 6;
    double cos_angle = cos(angle);
    double sin_angle = sin(angle);

    // Initialize the 3D grid and apply rotations
    for (int x = 0; x < rows; x++) {
        for (int y = 0; y < cols; y++) {
			if (x == 5 && y > 1 && y < 6)
				p[x][y].z2D = 10 * data->d;
			else
            	p[x][y].z2D = 0; // Set initial Z-coordinate to 0
            p[x][y].x2D = (x - y) * cos_angle; // Apply simple projection formula
            p[x][y].y2D = (x + y) * sin_angle;

            // Apply rotations around both axes
            apply_rotation(&p[x][y], data->rot_x * angle, data->rot_y * angle);
        }
    }

    // Handle key presses for translation and zoom
    if (key == 65361) // Left arrow
        data->x1 -= 10;
    if (key == 65363) // Right arrow
        data->x1 += 10;
    if (key == 65364) // Down arrow
        data->y1 -= 10;
    if (key == 65362) // Up arrow
        data->y1 += 10;
    if (key == 65307) // Escape key
        exit(0);
    if (key == 65453) // Zoom out
        data->z -= 1;
    if (key == 65451) // Zoom in
        data->z += 1;

    // Create a new image for drawing
    data->img = mlx_new_image(data->mlx, WIDTH, HEIGHT);
    data->data = mlx_get_data_addr(data->img, &data->bpp, &data->s_l, &data->endian);

    // Draw the grid lines
    for (int i = 0; i < rows - 1; i++) {
        for (int j = 0; j < cols - 1; j++) {
            // Draw lines between adjacent points
            if (p[i][j].x2D * data->z + data->x1 >= 0 && p[i][j].y2D * data->z + data->y1 >= 0 &&
                p[i][j + 1].x2D * data->z + data->x1 >= 0 && p[i][j + 1].y2D * data->z + data->y1 >= 0 &&
                p[i][j].x2D * data->z + data->x1 <= WIDTH && p[i][j].y2D * data->z + data->y1 <= HEIGHT &&
                p[i][j + 1].x2D * data->z + data->x1 <= WIDTH && p[i][j + 1].y2D * data->z + data->y1 <= HEIGHT)
                draw_line(data->data, p[i][j].x2D * data->z + data->x1, p[i][j].y2D * data->z + data->y1,
                          p[i][j + 1].x2D * data->z + data->x1, p[i][j + 1].y2D * data->z + data->y1, 0xFF0000, data->bpp, data->s_l);
            if (p[i][j].x2D * data->z + data->x1 >= 0 && p[i][j].y2D * data->z + data->y1 >= 0 &&
                p[i + 1][j].x2D * data->z + data->x1 >= 0 && p[i + 1][j].y2D * data->z + data->y1 >= 0 &&
                p[i][j].x2D * data->z + data->x1 <= WIDTH && p[i][j].y2D * data->z + data->y1 <= HEIGHT &&
                p[i + 1][j].x2D * data->z + data->x1 <= WIDTH && p[i + 1][j].y2D * data->z + data->y1 <= HEIGHT)
                draw_line(data->data, p[i][j].x2D * data->z + data->x1, p[i][j].y2D * data->z + data->y1,
                          p[i + 1][j].x2D * data->z + data->x1, p[i + 1][j].y2D * data->z + data->y1, 0xFF0000, data->bpp, data->s_l);
        }
    }

    // Update the window with the new image
    mlx_put_image_to_window(data->mlx, data->win, data->img, 0, 0);
    mlx_destroy_image(data->mlx, data->img);

    // Free allocated memory
    for (int i = 0; i < rows; i++) {
        free(p[i]);
    }
    free(p);

    return 0;
}

int handle_close(void *param)
{
    (void)param; // If param is unused, explicitly cast it to void
    printf("Window closed.\n");
    exit(0); // Exit the program when the window is closed
    return (0);
}

int main()
{
	t_s1 data;
	data.mlx = mlx_init();
	data.win = mlx_new_window(data.mlx, WIDTH, HEIGHT, "test");
	data.x0 = 100; data.y0 = 100; data.x1 = 500;
	data.y1 = 500;
	data.z = 10;
	data.rot_x = 0;
	data.rot_y = 0;
	data.d = 1;
	mlx_hook(data.win, 2, 1L<<0, test, &data);
	mlx_hook(data.win, 17, 0, handle_close,NULL);
	//mlx_loop_hook(data.mlx);
	mlx_loop(data.mlx);
	/*t_d data;
	data.mlx = mlx_init();
    data.win = mlx_new_window(data.mlx, 800, 600, "mlx_loop_hook Example");
    data.x = 0;

    // Register the update function to be called continuously
    mlx_loop_hook(data.mlx, update, &data);

    // Start the event loop
    mlx_loop(data.mlx);*/
}
