#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "minilibx/mlx.h"
#include <math.h>
#define ESC 65307
#define W 119
#define D 100
#define E 101
#define S 115
#define A 97
#define P 112
#define ARROW_R 65363
#define ARROW_L 65361
#define PI 3.141592653589793238
#define BLOCK_SIZE 32
#define RAYS 100

typedef struct s_wall
{
	double	angle;
	double	x;
	double	y;
	int		count;
	int		row;
	int		col;
	int		wall_front;
	int		wall_back;
	int		wall_right;
	int		wall_left;
}				t_wall;

typedef struct s_rays
{
	double	dist;
	double	ang;
	double	x;
	double	y;
	double	width;
	double	height;
}				t_rays;

typedef struct s_player
{
	double	x_co;
	double	y_co;
	double	ang;
	double	dx;
	double	dy;
	t_wall	wall;
}				t_player;

typedef struct s_vars
{
	char		**map;
	void		*mlx;
	void		*win;
	void		*img;
	char		*addr;
	int			bpp;
	int			len;
	int			end;
	int			width;
	int			height;
	int			pause;
	t_player	p;
	t_rays		ray[120];
}				t_vars;

void	ft_putstr_map(char **map)
{
	int	i;

	i = 0;
	while (map[i] != NULL)
	{
		printf("%s\n", map[i]);
		i++;
	}
	printf("\n");
}

void	my_pixel_put(t_vars *vars, int x, int y, int rgb)
{
	char *draw;

	draw = vars->addr + (y * vars->len + x * (vars->bpp / 8));
	*(unsigned int *)draw = rgb;
}

void	draw_point(t_vars *vars, double x, double y)
{
	int	i;
	int	j;

	i = 5;
	j = 3;
	while (j > 0)
	{
		i = 3;
		while (i > 0)
		{
			mlx_pixel_put(vars->mlx, vars->win, x + i, y + j, 0x00FF00);
			// my_pixel_put(vars, x + i, y + j, 0x00FF00);
			i--;
		}
		j--;
	}
}

int	ft_2d_len(char **str)
{
	int	count;

	count = 0;
	while (str[count] != NULL)
		count++;
	return (count);
}

char	**get_map2d(void)
{
	char	**map;

	map = malloc(sizeof(char *) * 9);
	map[0] = strdup("11111111");
	map[1] = strdup("10000001");
	map[2] = strdup("10000001");
	map[3] = strdup("10001001");
	map[4] = strdup("10010101");
	map[5] = strdup("1000P001");
	map[6] = strdup("10000001");
	map[7] = strdup("11111111");
	map[8] = NULL;
	return (map);
}

void	init(t_vars *vars)
{
	vars->addr = NULL;
	vars->height = 0;
	vars->img = NULL;
	vars->map = NULL;
	vars->mlx = NULL;
	vars->width = 0;
	vars->win = NULL;
	vars->p.x_co = 0;
	vars->p.y_co = 0;
	vars->p.ang = 0;
	vars->p.dx = 0;
	vars->p.dy = 0;
	vars->pause = 0;
	vars->p.wall.wall_back = 0;
	vars->p.wall.wall_front = 0;
	vars->p.wall.wall_left = 0;
	vars->p.wall.wall_right = 0;
}

void	init_wall_coll(t_vars *vars)
{
	vars->p.wall.count = 7;
	vars->p.wall.row = 0;
	vars->p.wall.col = 0;
	vars->p.wall.x = 0;
	vars->p.wall.y = 0;
	vars->p.wall.angle = 0;
}

void	wall_coll_forwrd(t_vars *vars)
{
	init_wall_coll(vars);
	vars->p.wall.wall_front = 0;
	vars->p.wall.angle = vars->p.ang - (30 * PI / 180);
	if (vars->p.wall.angle < 0)
		vars->p.wall.angle += 2 * PI;
	while (vars->p.wall.count > 0)
	{
		vars->p.wall.x = vars->p.x_co + (cos(vars->p.wall.angle) * 0.125);
		vars->p.wall.y = vars->p.y_co + (sin(vars->p.wall.angle) * 0.125);
		vars->p.wall.row = (int)vars->p.wall.x;
		vars->p.wall.col = (int)vars->p.wall.y;
		if (!vars->map[vars->p.wall.col][vars->p.wall.row]
			|| (vars->map[vars->p.wall.col][vars->p.wall.row]
			&& vars->map[vars->p.wall.col][vars->p.wall.row] == '1'))
			vars->p.wall.wall_front = 1;
		draw_point(vars, (vars->p.wall.x * 87) + 700, vars->p.wall.y * 87);
		vars->p.wall.angle += (10 * PI / 180);
		if (vars->p.wall.angle > 2 * PI)
			vars->p.wall.angle -= 2 * PI;
		vars->p.wall.count--;
	}
}

void	wall_coll_bckwrd(t_vars *vars)
{
	init_wall_coll(vars);
	vars->p.wall.wall_back = 0;
	vars->p.wall.angle = vars->p.ang - (30 * PI / 180) - PI;
	while (vars->p.wall.count > 0)
	{
		vars->p.wall.x = vars->p.x_co + (cos(vars->p.wall.angle) * 0.125);
		vars->p.wall.y = vars->p.y_co + (sin(vars->p.wall.angle) * 0.125);
		vars->p.wall.row = (int)vars->p.wall.x;
		vars->p.wall.col = (int)vars->p.wall.y;
		if (!vars->map[vars->p.wall.col][vars->p.wall.row]
			|| (vars->map[vars->p.wall.col][vars->p.wall.row]
			&& vars->map[vars->p.wall.col][vars->p.wall.row] == '1'))
			vars->p.wall.wall_back = 1;
		draw_point(vars, (vars->p.wall.x * 87) + 700, vars->p.wall.y * 87);
		vars->p.wall.angle += (10 * PI / 180);
		if (vars->p.wall.angle > 2 * PI)
			vars->p.wall.angle -= 2 * PI;
		vars->p.wall.count--;
	}
}

void	wall_coll_left(t_vars *vars)
{
	init_wall_coll(vars);
	vars->p.wall.wall_left = 0;
	vars->p.wall.angle = vars->p.ang - (30 * PI / 180);
	if (vars->p.wall.angle < 0)
		vars->p.wall.angle += 2 * PI;
	vars->p.wall.angle += (3 * PI / 2);
	if (vars->p.wall.angle > 2 * PI)
		vars->p.wall.angle -= 2 * PI;
	while (vars->p.wall.count > 0)
	{
		vars->p.wall.x = vars->p.x_co + (cos(vars->p.wall.angle) * 0.125);
		vars->p.wall.y = vars->p.y_co + (sin(vars->p.wall.angle) * 0.125);
		vars->p.wall.row = (int)vars->p.wall.x;
		vars->p.wall.col = (int)vars->p.wall.y;
		if (!vars->map[vars->p.wall.col][vars->p.wall.row]
			|| (vars->map[vars->p.wall.col][vars->p.wall.row]
			&& vars->map[vars->p.wall.col][vars->p.wall.row] == '1'))
			vars->p.wall.wall_left = 1;
		draw_point(vars, (vars->p.wall.x * 87) + 700, vars->p.wall.y * 87);
		vars->p.wall.angle += (10 * PI / 180);
		if (vars->p.wall.angle > 2 * PI)
			vars->p.wall.angle -= 2 * PI;
		vars->p.wall.count--;
	}
}

void	wall_coll_right(t_vars *vars)
{
	init_wall_coll(vars);
	vars->p.wall.wall_right = 0;
	vars->p.wall.angle = vars->p.ang - (30 * PI / 180);
	if (vars->p.wall.angle < 0)
		vars->p.wall.angle += 2 * PI;
	vars->p.wall.angle += (PI / 2);
	if (vars->p.wall.angle > 2 * PI)
		vars->p.wall.angle -= 2 * PI;
	while (vars->p.wall.count > 0)
	{
		vars->p.wall.x = vars->p.x_co + (cos(vars->p.wall.angle) * 0.125);
		vars->p.wall.y = vars->p.y_co + (sin(vars->p.wall.angle) * 0.125);
		vars->p.wall.row = (int)vars->p.wall.x;
		vars->p.wall.col = (int)vars->p.wall.y;
		if (!vars->map[vars->p.wall.col][vars->p.wall.row]
			|| (vars->map[vars->p.wall.col][vars->p.wall.row]
			&& vars->map[vars->p.wall.col][vars->p.wall.row] == '1'))
			vars->p.wall.wall_right = 1;
		draw_point(vars, (vars->p.wall.x * 87) + 700, vars->p.wall.y * 87);
		vars->p.wall.angle += (10 * PI / 180);
		if (vars->p.wall.angle > 2 * PI)
			vars->p.wall.angle -= 2 * PI;
		vars->p.wall.count--;
	}
}

int	handle_wall_collision(t_vars *vars)
{
	wall_coll_forwrd(vars);
	wall_coll_bckwrd(vars);
	wall_coll_left(vars);
	wall_coll_right(vars);
	return (0);
}

int	check_wall(t_vars *vars, double x, double y)
{
	int	col;
	int	row;

	if ((y >= 0) && x < strlen(vars->map[(int)(y)]))
	{
		row = x;
		col = y;
		if (vars->map[col][row] == '1')
			return (1);
		return (0);
	}
	return (1);
}

double	sq(double num)
{
	return (num * num);
}

double	get_dist(double x_one, double y_one, double x_two, double y_two)
{
	double	dist;

	dist = sqrt(sq(x_two - x_one) + sq(y_two - y_one));
	return (dist);
}

void	print_points(double x, double y)
{
	printf("X: %f, Y: %f\n", x, y);
}

void	make_line(t_vars *vars, float x, float y)
{
	double	m;
	double	x1;
	double	x2;
	double	y1;
	double	y2;
	double	incpt;

	x1 = vars->p.x_co * 87.5;
	y1 = vars->p.y_co * 87.5;
	x2 = x;
	y2 = y;
	m = (y2 - y1) / (x2 - x1);
	incpt = y1 - (m * x1);
	if (x1 < x2)
	{
		while(x1 <= x2)
		{
			draw_point(vars, x1 + 700, (m * x1) + incpt);
			x1++;
		}
	}
	else
	{
		while (x1 >= x2)
		{
			draw_point(vars, x1 + 700, (m * x1) + incpt);
			x1--;
		}
	}
}

void	make_line_vertical(t_vars *vars, double x, double y)
{
	double	m;
	double	x1;
	double	x2;
	double	y1;
	double	y2;
	double	incpt;

	x1 = vars->p.x_co * 87.5;
	y1 = vars->p.y_co * 87.5;
	x2 = x;
	y2 = y;
	m = (y2 - y1) / (x2 - x1);
	printf("Slope: %f\n", m);
	incpt = y1 - (m * x1);
	if (y1 < y2)
	{
		while(y1 <= y2)
		{
			draw_point(vars, (y1 - incpt) / m, x1);
			y1++;
		}
	}
	else
	{
		while (y1 >= y2)
		{
			draw_point(vars, (y1 - incpt) / m, x1);
			y1--;
		}
	}
}

void	draw_width(t_vars *vars, double x, double y)
{
	int	i;
	int	j;

	j = 2;
	while (j > 0)
	{
		i = 7;
		while (i >= 0)
		{
			if (y + j < 700 && y + j > 0 && x + i < 700 && x + i > 0)
				my_pixel_put(vars, x + i, y + j, 0x440000);
			i--;
		}
		j--;
	}
}

void	make_wall(t_vars *vars, double *x, double *y)
{
	int	count;
	static int	index;

	if (index == 120)
		index = 0;
	count = vars->ray[index].height;
	draw_width(vars, *x, *y);
	while (count > 0)
	{
		draw_width(vars, *x, *y);
		(*y) += 1;
		count--;
	}
	index++;
}

int	edge_case(double x, double y, int g_x, int g_y, t_vars *vars)
{
	int	c_x;
	int	c_y;

	c_x = (int)x;
	c_y = (int)y;
	printf("Check X: %d, Check Y: %d\n", c_x, c_y);
	// printf("Double X: %d, Double Y: %d, Grid_X: %d, Grid_Y: %d\n", (int)x, (int)y, g_x, g_y);
	// printf("Player X: %d, Player Y: %d\n", (int)vars->p.x_co, (int)vars->p.y_co);
	if (vars->p.x_co > (int)x && vars->p.y_co > (int)y)
	{
		if ((c_x + 1 < strlen(vars->map[c_y])) && (vars->map[c_y][c_x + 1] == '1'))
		{
			if ((c_y + 1 < ft_2d_len(vars->map)) && (vars->map[c_y + 1][c_x] == '1'))
				return (1);
		}
		return(0);
	}
	else if (vars->p.x_co < (int)x && vars->p.y_co < (int)y)
	{
		if ((c_x - 1 >= 0) && (vars->map[c_y][c_x - 1] == '1'))
		{
			if ((c_y - 1 >= 0) && (vars->map[c_y - 1][c_x] == '1'))
				return (1);
		}
		return(0);
	}
	else if (vars->p.x_co < (int)x && vars->p.y_co > (int)y)
	{
		if ((c_x - 1 >= 0) && (vars->map[c_y][c_x - 1] == '1'))
		{
			if ((c_y + 1 < ft_2d_len(vars->map)) && (vars->map[c_y + 1][c_x] == '1'))
				return (1);
		}
		return(0);
	}
	else if (vars->p.x_co > (int)x && vars->p.y_co < (int)y)
	{
		if ((c_x + 1 < strlen(vars->map[c_y])) && (vars->map[c_y][c_x + 1] == '1'))
		{
			if ((c_y - 1 >= 0) && (vars->map[c_y - 1][c_x] == '1'))
				return (1);
		}
		return(0);
	}
	return (0);
}

void	print_rays(t_vars *vars)
{
	double	angle;
	double	x;
	double	y;
	double	old_x;
	double	old_y;
	int		wall;
	int		count;
	double	x1;

	angle = vars->p.ang - (33 * (PI / 180));
	if (angle < 0)
		angle += 2 * PI;
	count = 120;
	x1 = 0;
	while (count > 0)
	{
		x = vars->p.x_co;
		y = vars->p.y_co;
		wall = 0;
		int	grid_x;
		int	grid_y;
		grid_x = (int)x;
		grid_y = (int)y;
		while (!wall)
		{
			x += (cos(angle) * 0.01);
			y += (sin(angle) * 0.01);
			wall = check_wall(vars, x, y);
			if (!wall && (grid_x != (int)x && grid_y != (int)y))
				wall = edge_case(x, y, grid_x, grid_y, vars);
		}
		vars->ray[120 - count].x = x;
		vars->ray[120 - count].y = y;
		vars->ray[120 - count].dist = get_dist(vars->p.x_co, vars->p.y_co, x, y);
		vars->ray[120 - count].height = (BLOCK_SIZE * 700) / (vars->ray[120 - count].dist * 87.5);
		// printf("Dist: %f\n", vars->ray[120 - count].height);
		// printf("Ang: %f\n", angle);
		if (vars->ray[120 - count].height > 700)
			vars->ray[120 - count].height = 700;
		// double	row;
		// double	col;
		// row = (x * 87.5) + 700;
		// printf("Row: %f\n", row);
		// col = y * 87.5;
		draw_point(vars, (x * 87) + 700, y * 87);
		make_line(vars, x * 87, y * 87);
		y = 350 - (vars->ray[120 - count].height / 2);
		make_wall(vars, &x1, &y);
		angle += 0.009603175; // (PI / 180) * (70 / RAYS)
		if (angle > 2 * PI)
			angle -= 2 * PI;
		x1 += 6.38; // 11.6 * (70 / RAYS)
		count--;
	}
}

void	free_map(char **map)
{
	int	i;

	i = 0;
	while (map[i] != NULL)
	{
		free(map[i]);
		i++;
	}
	free(map);
}

void	exit_game(t_vars *vars)
{
	mlx_destroy_window(vars->mlx, vars->win);
	free_map(vars->map);
	exit(0);
}

void	move_frwrd(t_vars *vars)
{
	int	row;
	int	col;

	vars->p.dx = cos(vars->p.ang) * 0.0556;
	vars->p.dy = sin(vars->p.ang) * 0.0556;
	if ((vars->p.y_co + vars->p.dy >= 0)
		&& vars->p.x_co + vars->p.dx
		< strlen(vars->map[(int)(vars->p.y_co + vars->p.dy)]))
	{
		row = vars->p.x_co + vars->p.dx;
		col = vars->p.y_co + vars->p.dy;
		if (vars->map[col][row] == '1' || vars->p.wall.wall_front)
		{
			draw_point(vars, (vars->p.x_co * 87) + 700, (vars->p.y_co * 87));
			return ;
		}
		vars->map[(int)vars->p.y_co][(int)vars->p.x_co] = '0';
		vars->p.x_co += vars->p.dx;
		vars->p.y_co += vars->p.dy;
		vars->map[col][row] = 'P';
	}
}

void	move_right(t_vars *vars)
{
	int	row;
	int	col;

	vars->p.dx = cos((PI / 2) + (vars->p.ang)) * 0.0556;
	vars->p.dy = sin((PI / 2) + (vars->p.ang)) * 0.0556;
	if ((vars->p.y_co + vars->p.dy >= 0)
		&& vars->p.x_co + vars->p.dx
		< strlen(vars->map[(int)(vars->p.y_co + vars->p.dy)]))
	{
		row = vars->p.x_co + vars->p.dx;
		col = vars->p.y_co + vars->p.dy;
		if (vars->map[col][row] == '1' || vars->p.wall.wall_right)
		{
			// draw_point(vars, (vars->p.x_co * 87), (vars->p.y_co * 87));
			return ;
		}
		vars->map[(int)vars->p.y_co][(int)vars->p.x_co] = '0';
		vars->p.x_co += vars->p.dx;
		vars->p.y_co += vars->p.dy;
		vars->map[col][row] = 'P';
	}
}

void	move_bckwrd(t_vars *vars)
{
	int	row;
	int	col;

	vars->p.dx = cos(vars->p.ang) * 0.0556;
	vars->p.dy = sin(vars->p.ang) * 0.0556;
	if ((vars->p.y_co - vars->p.dy >= 0)
		&& vars->p.x_co - vars->p.dx
		< strlen(vars->map[(int)(vars->p.y_co - vars->p.dy)]))
	{
		row = vars->p.x_co - vars->p.dx;
		col = vars->p.y_co - vars->p.dy;
		if (vars->map[col][row] == '1' || vars->p.wall.wall_back)
		{
			// draw_point(vars, (vars->p.x_co * 87), (vars->p.y_co * 87));
			return ;
		}
		vars->map[(int)vars->p.y_co][(int)vars->p.x_co] = '0';
		vars->p.x_co -= vars->p.dx;
		vars->p.y_co -= vars->p.dy;
		vars->map[col][row] = 'P';
	}
}

void	move_left(t_vars *vars)
{
	int	row;
	int	col;

	vars->p.dx = cos((PI / 2) + (vars->p.ang)) * 0.0556;
	vars->p.dy = sin((PI / 2) + (vars->p.ang)) * 0.0556;
	if ((vars->p.y_co - vars->p.dy >= 0)
		&& vars->p.x_co - vars->p.dx
		< strlen(vars->map[(int)(vars->p.y_co - vars->p.dy)]))
	{
		row = vars->p.x_co - vars->p.dx;
		col = vars->p.y_co - vars->p.dy;
		if (vars->map[col][row] == '1' || vars->p.wall.wall_left)
		{
			// draw_point(vars, (vars->p.x_co * 87), (vars->p.y_co * 87));
			return ;
		}
		vars->map[(int)vars->p.y_co][(int)vars->p.x_co] = '0';
		vars->p.x_co -= vars->p.dx;
		vars->p.y_co -= vars->p.dy;
		vars->map[col][row] = 'P';
	}
}

void	print_image(t_vars *vars)
{
	int	j;
	int	i;
	int	col;

	j = 0;
	col = 0x00AAFF;
	while (j < 700)
	{
		i = 0;
		if (j == 350)
			col = 0x211100;
		while (i < 700)
		{
			my_pixel_put(vars, i, j, col);
			i++;
		}
		j++;
	}
}

void	print_window(t_vars *vars)
{
	int	j;
	int	i;
	void	*img;

	j = 0;
	img = mlx_xpm_file_to_image(vars->mlx, "xpm_files/wall.xpm",
			&vars->width, &vars->height);
	while (vars->map[j] != NULL)
	{
		i = 0;
		while (vars->map[j][i])
		{
			if (vars->map[j][i] == '1')
				mlx_put_image_to_window(vars->mlx, vars->win,
					img, (i * 87) + 700, j * 87);
			i++;
		}
		j++;
	}
	mlx_destroy_image(vars->mlx, img);
	print_image(vars);
}

void	move_player(int keycode, t_vars *vars)
{
	if (keycode == ESC)
		exit_game(vars);
	else if (keycode == W)
		move_frwrd(vars);
	else if (keycode == S)
		move_bckwrd(vars);
	else if (keycode == A)
		move_left(vars);
	else if (keycode == D)
		move_right(vars);
	else if (keycode == P)
	{
		if (vars->pause == 1)
			vars->pause = 0;
		else
			vars->pause = 1;
	}
}

void	rotate_player(int keycode, t_vars *vars)
{
	if (keycode == ARROW_R)
		vars->p.ang += 0.1;
	if (vars->p.ang > 2 * PI)
		vars->p.ang -= 2 * PI;
	if (keycode == ARROW_L)
		vars->p.ang -= 0.1;
	if (vars->p.ang < 2 * PI)
		vars->p.ang += 2 * PI;
}

int	key_hook(int keycode, t_vars *vars)
{
	mlx_destroy_image(vars->mlx, vars->img);
	vars->img = mlx_new_image(vars->mlx, 700, 700);
	mlx_clear_window(vars->mlx, vars->win);
	print_window(vars);
	move_player(keycode, vars);
	rotate_player(keycode, vars);
	draw_point(vars, (vars->p.x_co * 87) + 700, (vars->p.y_co * 87));
	handle_wall_collision(vars);
	print_rays(vars);
	mlx_put_image_to_window(vars->mlx, vars->win, vars->img, 0, 0);
	// ft_putstr_map(vars->map);
	return (0);
}

int	mouse_hook(int x, int y, t_vars *vars)
{
	if (vars->pause == 0)
	{
		mlx_mouse_get_pos(vars->mlx, vars->win, &x, &y);
		if (x < 350)
			key_hook(ARROW_L, vars);
		else if (x > 350)
			key_hook(ARROW_R, vars);
		mlx_mouse_move(vars->mlx, vars->win, 350, 350);
		mlx_mouse_hide(vars->mlx, vars->win);
	}
	else
		mlx_mouse_show(vars->mlx, vars->win);
	return (0);
}

void	print_player(t_vars *vars)
{
	int	j;
	int	i;

	j = 0;
	vars->p.ang = 270 * (PI / 180);
	while (vars->map[j] != NULL)
	{
		i = 0;
		while (vars->map[j][i])
		{
			if (vars->map[j][i] == 'P')
				break ;
			i++;
		}
		if (vars->map[j][i] == 'P')
			break ;
		j++;
	}
	vars->p.x_co = (i + 0.5);
	vars->p.y_co = j + 0.5;
	draw_point(vars, (700 + (vars->p.x_co * 87)), (vars->p.y_co * 87));
	handle_wall_collision(vars);
}

int	main(void)
{
	t_vars	vars;

	init(&vars);
	vars.map = get_map2d();
	vars.mlx = mlx_init();
	vars.win = mlx_new_window(vars.mlx, 1400, 700, "Cub3D");
	vars.img = mlx_new_image(vars.mlx, 700, 700);
	vars.addr = mlx_get_data_addr(vars.img, &vars.bpp, &vars.len, &vars.end);
	print_window(&vars);
	print_player(&vars);
	print_rays(&vars);
	mlx_hook(vars.win, 2, (1L << 0), key_hook, &vars);
	// mlx_hook(vars.win, 6, (1L << 6), mouse_hook, &vars);
	mlx_put_image_to_window(vars.mlx, vars.win, vars.img, 0, 0);
	mlx_loop(vars.mlx);
	return (0);
}