#pragma once
#include <cmath>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <iterator>

using std::abs;
using std::max;
using std::vector;

#include "HexMap.h"

const double PI = 3.14;

struct Point
{
	const double x;
	const double y;
	Point(double x_, double y_) : x(x_), y(y_) {}
};


struct Hex
{
	const int q;
	const int r;
	const int s;
	Hex(int q_, int r_, int s_) : q(q_), r(r_), s(s_) {}
};


struct FractionalHex
{
	const double q;
	const double r;
	const double s;
	FractionalHex(double q_, double r_, double s_) : q(q_), r(r_), s(s_) {}
};


struct OffsetCoord
{
	const int col;
	const int row;
	OffsetCoord(int col_, int row_) : col(col_), row(row_) {}
};


struct Orientation
{
	const double f0;
	const double f1;
	const double f2;
	const double f3;
	const double b0;
	const double b1;
	const double b2;
	const double b3;
	const double start_angle;
	Orientation(double f0_, double f1_, double f2_, double f3_, double b0_, double b1_, double b2_, double b3_, double start_angle_) : f0(f0_), f1(f1_), f2(f2_), f3(f3_), b0(b0_), b1(b1_), b2(b2_), b3(b3_), start_angle(start_angle_) {}
};


struct Layout
{
	const Orientation orientation;
	const Point size;
	const Point origin;
	Layout(Orientation orientation_, Point size_, Point origin_) : orientation(orientation_), size(size_), origin(origin_) {}
};


// Forward declarations


Hex hex_add(Hex a, Hex b)
{
	return Hex(a.q + b.q, a.r + b.r, a.s + b.s);
}


Hex hex_subtract(Hex a, Hex b)
{
	return Hex(a.q - b.q, a.r - b.r, a.s - b.s);
}


Hex hex_scale(Hex a, int k)
{
	return Hex(a.q * k, a.r * k, a.s * k);
}


const vector<Hex> hex_directions = { Hex(1, 0, -1), Hex(1, -1, 0), Hex(0, -1, 1), Hex(-1, 0, 1), Hex(-1, 1, 0), Hex(0, 1, -1) };
Hex hex_direction(int direction)
{
	return hex_directions[direction];
}


Hex hex_neighbor(Hex hex, int direction)
{
	return hex_add(hex, hex_direction(direction));
}


const vector<Hex> hex_diagonals = { Hex(2, -1, -1), Hex(1, -2, 1), Hex(-1, -1, 2), Hex(-2, 1, 1), Hex(-1, 2, -1), Hex(1, 1, -2) };
Hex hex_diagonal_neighbor(Hex hex, int direction)
{
	return hex_add(hex, hex_diagonals[direction]);
}


int hex_length(Hex hex)
{
	return int((abs(hex.q) + abs(hex.r) + abs(hex.s)) / 2);
}


int hex_distance(Hex a, Hex b)
{
	return hex_length(hex_subtract(a, b));
}



Hex hex_round(FractionalHex h)
{
	int q = int(round(h.q));
	int r = int(round(h.r));
	int s = int(round(h.s));
	double q_diff = abs(q - h.q);
	double r_diff = abs(r - h.r);
	double s_diff = abs(s - h.s);
	if ((q_diff > r_diff) & (q_diff > s_diff))
	{
		q = -r - s;
	}
	else
		if (r_diff > s_diff)
		{
			r = -q - s;
		}
		else
		{
			s = -q - r;
		}
	return Hex(q, r, s);
}


FractionalHex hex_lerp(FractionalHex a, FractionalHex b, double t)
{
	return FractionalHex(a.q * (1 - t) + b.q * t, a.r * (1 - t) + b.r * t, a.s * (1 - t) + b.s * t);
}


vector<Hex> hex_linedraw(Hex a, Hex b)
{
	int N = hex_distance(a, b);
	FractionalHex a_nudge = FractionalHex(a.q + 0.000001, a.r + 0.000001, a.s - 0.000002);
	FractionalHex b_nudge = FractionalHex(b.q + 0.000001, b.r + 0.000001, b.s - 0.000002);
	vector<Hex> results = {};
	double step = 1.0 / max(N, 1);
	for (int i = 0; i <= N; i++)
	{
		results.push_back(hex_round(hex_lerp(a_nudge, b_nudge, step * i)));
	}
	return results;
}



const int EVEN = 1;
const int ODD = -1;
OffsetCoord qoffset_from_cube(int offset, Hex h)
{
	int col = h.q;
	int row = h.r + int((h.q + offset * (h.q & 1)) / 2);
	return OffsetCoord(col, row);
}


Hex qoffset_to_cube(int offset, OffsetCoord h)
{
	int q = h.col;
	int r = h.row - int((h.col + offset * (h.col & 1)) / 2);
	int s = -q - r;
	return Hex(q, r, s);
}


OffsetCoord roffset_from_cube(int offset, Hex h)
{
	int col = h.q + int((h.r + offset * (h.r & 1)) / 2);
	int row = h.r;
	return OffsetCoord(col, row);
}


Hex roffset_to_cube(int offset, OffsetCoord h)
{
	int q = h.col - int((h.row + offset * (h.row & 1)) / 2);
	int r = h.row;
	int s = -q - r;
	return Hex(q, r, s);
}




const Orientation layout_pointy = Orientation(sqrt(3.0), sqrt(3.0) / 2.0, 0.0, 3.0 / 2.0, sqrt(3.0) / 3.0, -1.0 / 3.0, 0.0, 2.0 / 3.0, 0.5);
const Orientation layout_flat = Orientation(3.0 / 2.0, 0.0, sqrt(3.0) / 2.0, sqrt(3.0), 2.0 / 3.0, 0.0, -1.0 / 3.0, sqrt(3.0) / 3.0, 0.0);
Point hex_to_pixel(Layout layout, Hex h)
{
	Orientation M = layout.orientation;
	Point size = layout.size;
	Point origin = layout.origin;
	double x = (M.f0 * h.q + M.f1 * h.r) * size.x;
	double y = (M.f2 * h.q + M.f3 * h.r) * size.y;
	return Point(x + origin.x, y + origin.y);
}


FractionalHex pixel_to_hex(Layout layout, Point p)
{
	Orientation M = layout.orientation;
	Point size = layout.size;
	Point origin = layout.origin;
	Point pt = Point((p.x - origin.x) / size.x, (p.y - origin.y) / size.y);
	double q = M.b0 * pt.x + M.b1 * pt.y;
	double r = M.b2 * pt.x + M.b3 * pt.y;
	return FractionalHex(q, r, -q - r);
}


Point hex_corner_offset(Layout layout, int corner)
{
	Orientation M = layout.orientation;
	Point size = layout.size;
	double angle = 2.0 * PI * (M.start_angle - corner) / 6;
	return Point(size.x * cos(angle), size.y * sin(angle));
}


vector<Point> polygon_corners(Layout layout, Hex h)
{
	vector<Point> corners = {};
	Point center = hex_to_pixel(layout, h);
	for (int i = 0; i < 6; i++)
	{
		Point offset = hex_corner_offset(layout, i);
		corners.push_back(Point(center.x + offset.x, center.y + offset.y));
	}
	return corners;
}
