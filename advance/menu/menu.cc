/*
 * This file is part of the Advance project.
 *
 * Copyright (C) 1999, 2000, 2001, 2002, 2003, 2005, 2009 Andrea Mazzoleni
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details. 
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "portable.h"

#include "game.h"
#include "text.h"
#include "play.h"
#include "menu.h"

#include "advance.h"

#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

static adv_font* int_font_info_1 = 0;
static adv_font* int_font_info_2 = 0;
static adv_font* int_font_info_3 = 0;
static adv_font* int_font_info_4 = 0;
static adv_font* int_font_info_5 = 0;
static adv_font* int_font_list = 0;
static adv_font* int_font_menu = 0;
static bool is_loaded = false;

// ------------------------------------------------------------------------
// Menu entry

string menu_entry::category(sort_item_func* category_extract)
{
	if (has_game())
		return category_extract(game_get());
	else
		return desc_get();
}

menu_entry::menu_entry(const game* Ag, const unsigned Aident)
{
	g = Ag;
	ident = Aident;
}

menu_entry::menu_entry(const string& Adesc)
{
	g = 0;
	desc = Adesc;
	ident = 0;
}

// ------------------------------------------------------------------------
// Menu draw

static inline bool issep(char c)
{
	return isspace(c) || (c == '-');
}

void draw_menu_game_center(const game_set& gar, const game& g, int x, int y, int dx, int dy, bool selected, merge_t merge)
{
	string s;
	if (g.emulator_get()->tree_get())
		s = g.description_tree_get();
	else
		s = g.description_get();

	unsigned ident = int_font_dx_get()/2;

	int_color color;
	int_color color_first;
	int_color color_in;
	int_color color_back;
	const game* gb;
	if (g.emulator_get()->tree_get())
		gb = &g.clone_best_get();
	else
		gb = &g;
	if (gb->play_get() != play_preliminary && gb->present_tree_get()) {
		color = selected ? COLOR_MENU_SELECT : COLOR_MENU_NORMAL;
		color_first = selected ? COLOR_MENU_TAG_SELECT : COLOR_MENU_TAG;
		color_in = selected ? COLOR_MENU_HIDDEN_SELECT : COLOR_MENU_HIDDEN;
		color_back = COLOR_MENU_NORMAL;
	} else {
		color = selected ? COLOR_MENU_HIDDEN_SELECT : COLOR_MENU_HIDDEN;
		color_first = color;
		color_in = color;
		color_back = COLOR_MENU_HIDDEN;
	}

	// search the split of the first word
	unsigned str_pos = 0;
	unsigned py = 0;
	bool in = false;

	while (dy >= py + int_font_dy_get()) {
		unsigned str_len = s.length() - str_pos;
		while (str_len > 0 && issep(s[str_pos])) {
			++str_pos;
			--str_len;
		}
		unsigned pixel_len = int_put_width(s.substr(str_pos, str_len));
		while (pixel_len > dx - 2*ident) {
			while (str_len > 0 && !issep(s[str_pos+str_len-1]))
				--str_len;
			while (str_len > 0 && issep(s[str_pos+str_len-1]))
				--str_len;
			pixel_len = int_put_width(s.substr(str_pos, str_len));
		}

		if (str_len == 0) {
			// retry splitting in any point
			str_len = s.length() - str_pos;
			if (str_len) {
				pixel_len = int_put_width(s.substr(str_pos, str_len));
				while (pixel_len > dx - 2*ident) {
					--str_len;
					pixel_len = int_put_width(s.substr(str_pos, str_len));
				}
			}
		}

		if (!str_len) {
			int_clear_alpha(x, y+py, dx, int_font_dy_get(), color_back.background);
		} else {
			int space_left = (dx - pixel_len) / 2;
			int ident_left = ident;
			if (ident_left > space_left)
				ident_left = space_left;
			int space_right = dx - pixel_len - space_left;
			int ident_right = ident;
			if (ident_right > space_right)
				ident_right = space_right;

			int_clear_alpha(x, y+py, space_left-ident_left, int_font_dy_get(), color_back.background);
			int_clear_alpha(x+space_left-ident_left, y+py, ident_left, int_font_dy_get(), color.background);
			int_put_special_alpha(in, x+space_left, y+py, pixel_len, s.substr(str_pos, str_len), color_first, color_in, color);
			int_clear_alpha(x+space_left+pixel_len, y+py, ident_right, int_font_dy_get(), color.background);
			int_clear_alpha(x+space_left+pixel_len+ident_right, y+py, space_right-ident_right, int_font_dy_get(), color_back.background);
		}

		color_first = color;
		py += int_font_dy_get();
		str_pos += str_len;
	}
}

void draw_menu_game_left(const game_set& gar, const game& g, int x, int y, int dx, bool selected, merge_t merge, unsigned ident)
{
	string s;
	if (g.emulator_get()->tree_get())
		s = g.description_tree_get();
	else
		s = g.description_get();

	ident += int_font_dx_get()/2;

	int_color color;
	int_color color_first;
	int_color color_in;
	const game* gb;
	if (g.emulator_get()->tree_get())
		gb = &g.clone_best_get();
	else
		gb = &g;
	if (gb->play_get() != play_preliminary && gb->present_tree_get()) {
		color = selected ? COLOR_MENU_SELECT : COLOR_MENU_NORMAL;
		color_first = selected ? COLOR_MENU_TAG_SELECT : COLOR_MENU_TAG;
		color_in = selected ? COLOR_MENU_HIDDEN_SELECT : COLOR_MENU_HIDDEN;
	} else {
		color = selected ? COLOR_MENU_HIDDEN_SELECT : COLOR_MENU_HIDDEN;
		color_first = color;
		color_in = color;
	}

	if (ident)
		int_clear_alpha(x, y, ident, int_font_dy_get(), color.background);

	bool in = false;
	int_put_special_alpha(in, x + ident, y, dx - ident, s, color_first, color_in, color);
}

void draw_list_game(const game& g, int x, int y, int dx, bool selected, const string align)
{
	string s;
	if (g.emulator_get()->tree_get())
		s = g.description_tree_get();
	else
		s = g.description_get();

	int ident = int_font_dx_get()/4;
	if (ident < 2)
		ident = 2;

	bool in = false;

	int_color color;
	int_color color_first;
	int_color color_in;
	const game* gb;
	if (g.emulator_get()->tree_get())
		gb = &g.clone_best_get();
	else
		gb = &g;
	if (gb->play_get() != play_preliminary && gb->present_tree_get()) {
		color = selected ? COLOR_MENU_SELECT : COLOR_MENU_NORMAL;
		color_first = selected ? COLOR_MENU_TAG_SELECT : COLOR_MENU_TAG;
		color_in = selected ? COLOR_MENU_HIDDEN_SELECT : COLOR_MENU_HIDDEN;
	} else {
		color = selected ? COLOR_MENU_HIDDEN_SELECT : COLOR_MENU_HIDDEN;
		color_first = color;
		color_in = color;
	}

	if (align == "center") {
		unsigned str_len = s.length();
		unsigned pixel_len = int_put_width(s);

		while (pixel_len > dx - 2*ident) {
			--str_len;
			while (str_len > 0 && issep(s[str_len-1]))
				--str_len;
			pixel_len = int_put_width(s.substr(0, str_len));
		}

		if (!str_len) {
			int_clear_alpha(x, y, dx, int_font_dy_get(), color.background);
		} else {
			int space_left = (dx - pixel_len) / 2;
			int space_right = dx - pixel_len - space_left;
			int_clear_alpha(x, y, space_left, int_font_dy_get(), color.background);
			int_put_special_alpha(in, x+space_left, y, pixel_len, s.substr(0, str_len), color_first, color_in, color);
			int_clear_alpha(x+space_left+pixel_len, y, space_right, int_font_dy_get(), color.background);
		}

	} else if (align == "right") {

		unsigned str_len = s.length();
		unsigned pixel_len = int_put_width(s);

		while (str_len > 0 && issep(s[str_len-1])) 
			--str_len;
		pixel_len = int_put_width(s.substr(0, str_len));

		while (pixel_len > dx - 2*ident) {
			--str_len;
			while (str_len > 0 && issep(s[str_len-1]))
				--str_len;
			pixel_len = int_put_width(s.substr(0, str_len)); 
		}

		if (!str_len) {
			int_clear_alpha(x, y, dx, int_font_dy_get(), color.background);
		} else {
			int space_left = dx - pixel_len - ident;
			int_clear_alpha(x, y, space_left, int_font_dy_get(), color.background);
			int_put_special_alpha(in, x+space_left, y, pixel_len, s.substr(0, str_len), color_first, color_in, color);
			int_clear_alpha(x+dx-ident, y, ident, int_font_dy_get(), color.background);
		}

	} else {
		int_clear_alpha(x, y, ident, int_font_dy_get(), color.background);
		int_put_special_alpha(in, x + ident, y, dx - ident, s, color_first, color_in, color);
	}

}

void draw_menu_empty(int x, int y, int dx, int dy, bool selected)
{
	int_color color;
	color = selected ? COLOR_MENU_HIDDEN_SELECT : COLOR_MENU_HIDDEN;
	int_clear_alpha(x, y, dx, dy, color.background);
}

void draw_menu_desc(const string& desc, int x, int y, int dx, bool selected)
{
	int_color color;
	color = selected ? COLOR_MENU_TAG_SELECT : COLOR_MENU_TAG;
	int_put_filled_alpha(x, y, dx, desc, color);
}

string reduce_string(const string& s, unsigned width)
{
	string r = s;

	while (int_put_width(r) > width && r.length()) {
		r.erase(r.length() - 1, 1);
	}

	return r;
}

string reduce_info_string(adv_font* font, const string& s, unsigned width)
{
	string r = s;

	int ident = int_font_info_dx_get(font)/4;
	if (ident < 2)
		ident = 2;
	
	while (int_put_info_width(font, r) > width - 2*ident && r.length()) {
		r.erase(r.length() - 1, 1);
	}

	return r;
}

void draw_tag_info(adv_font* font, const string s, int x, int y, int dx, const int_color& color, const string align)
{
	string r = reduce_info_string(font, s, dx);

	int pixel_len = int_put_info_width(font, r);
	int dy = int_font_info_dy_get(font);

	int ident = int_font_info_dx_get(font)/4;
	if (ident < 2) ident = 2;

	int space_left = 0;
	int space_right = 0;

	if (r.length()) {

		if (align == "center") {
			space_left = (dx - pixel_len) / 2;
			space_right = dx - pixel_len - space_left;
		} else if (align == "right") {
			space_left = dx - pixel_len - ident;
			space_right = ident;
		} else { 
			space_left = ident;
			space_right = dx - space_left - pixel_len;
		}

		int_clear_alpha(x, y, space_left, dy, color.background);
		int_put_info_alpha(font, x+space_left, y, r, color);
		int_clear_alpha(x+space_left+pixel_len, y, space_right, dy, color.background);

	} else {
		int_clear_alpha(x, y, dx, dy, color.background);
	}

}

void draw_tag_left(const string& s, int& xl, int& xr, int y, int sep, const int_color& color)
{
	string r = reduce_string(s, (xr - xl) - sep);

	int len = sep + int_put_width(r);

	if (len <= xr - xl) {
		int_put_alpha(xl, y, r, color);
		xl += len;
	}
}

void draw_tag_left_whole(const string& s, int& xl, int& xr, int y, int sep, const int_color& color)
{
	if (int_put_width(s) <= (xr - xl) - sep)
		draw_tag_left(s, xl, xr, y, sep, color);
}

void draw_tag_right(const string& s, int& xl, int& xr, int y, int sep, const int_color& color)
{
	string r = reduce_string(s, (xr - xl) - sep);

	int len = sep + int_put_width(r);

	if (len <= xr - xl) {
		int_put_alpha(xr - len + sep, y, r, color);
		xr -= len;
	}
}

void draw_tag_right_whole(const string& s, int& xl, int& xr, int y, int sep, const int_color& color)
{
	if (int_put_width(s) <= (xr - xl) - sep)
		draw_tag_right(s, xl, xr, y, sep, color);
}

string tag_info_get(const game* g, int gs, int ga, const string favorites, const string tag_info) {
	string info_tag = "";

	if (tag_info == "favorites") {
		info_tag = favorites;
	} else if (g) {
		if (tag_info == "time") {
			ostringstream os;
			unsigned time;
			if (g->emulator_get()->tree_get())
				time = g->time_tree_get();
			else
				time = g->time_get();
			os << setw(3) << setfill('0') << (time/3600) << ":" << setw(2) << setfill('0') << ((time/60)%60);
			info_tag = os.str();
		} else if (tag_info == "sessions") {
			ostringstream os;
			unsigned session;
			if (g->emulator_get()->tree_get())
				session = g->session_tree_get();
			else
				session = g->session_get();
			os << session;
			info_tag = os.str();
		} else if (tag_info == "description") {
			ostringstream os;
			if (g->emulator_get()->tree_get())
				os << g->description_tree_get();
			else
				os << g->description_get();
			info_tag = os.str();
		} else if (tag_info == "size") {
			ostringstream os;
			if (g->size_get() >= 10*1000*1000)
				os << setw(4) << g->size_get()/1000/1000 << "M";
			else
				os << setw(4) << g->size_get()/1000 << "k";
			info_tag = os.str();
		} else if (tag_info == "resolution") {
			if (g->sizex_get() && g->sizey_get()) {
				ostringstream os;
				os << g->sizex_get() << "x" << g->sizey_get();
				info_tag = os.str();
			}
		} else if (tag_info == "proportion") {
			if (g->aspectx_get() && g->aspecty_get()) {
				ostringstream os;
				os << g->aspectx_get() << ":" << g->aspecty_get();
				info_tag = os.str();
			}
		} else if (tag_info == "rom") {
			if (g->emulator_get()->tree_get())
				info_tag = (g->clone_best_get()).name_without_emulator_get();
			else
				info_tag = g->name_without_emulator_get();
		} else	if (tag_info == "emulator") {
			info_tag = g->emulator_get()->user_name_get();
		} else if (tag_info == "type"){
			info_tag = g->type_get()->name_get();
		} else if (tag_info == "refresh") {
			info_tag = g->refresh_get();
		} else	if (tag_info == "manufacturer") {
			info_tag = g->manufacturer_get();
		} else	if (tag_info == "year") {
			if (g->year_get().length())
				info_tag = g->year_get();
		} else	if (tag_info == "clones") {
			ostringstream os;
			unsigned clones;
			if (g->clone_get() >= 0)
				clones = g->clone_get();
			os << clones;
			info_tag = os.str();
		} else if (tag_info == "selected") {
			ostringstream os;
			os << gs;
			info_tag = os.str();
		} else if (tag_info == "games") {
			ostringstream os;
			os << ga;
			info_tag = os.str();
		} else if (tag_info == "game_favorites") {
			favorites_container fav = g->gfavorites_get();
			for(favorites_container::const_iterator i=fav.begin();i!=fav.end();++i) {
				info_tag += (*i) + "/";
			}
			if (info_tag != "")
				info_tag.erase(info_tag.length() - 1);
		} else if (tag_info == "clone") {
			if (g->parent_get())
				info_tag = "Clone of " + g->parent_get()->name_without_emulator_get();
			else if (g->clone_get()) {
				ostringstream os;
				if (g->clone_get() == 1)
					os << g->clone_get() << " Clone";
				else
					os << g->clone_get() << " Clones";
				info_tag = os.str();
			} else
				info_tag = "No Clones";
		}
	}
	return info_tag;
}

string info_get(const game* g, int gs, int ga, const string favorites, const string s_info) {
	int pos_ini = 0;
	int pos_fin = 0;
	string tag = "";
	string info = "";

	string s = s_info;

	while (s.length()) {
		pos_ini = s.find('%');
		if (pos_ini >= 0) {
			info += s.substr(0, pos_ini);
			s.erase(0, pos_ini+1);
			pos_fin = s.find('%');
			if (pos_fin > 0) {
				tag = s.substr(0, pos_fin);
				info += tag_info_get(g, gs, ga, favorites, tag);
				s.erase(0, pos_fin+1);
			}
		} else {
			info += s;
			s.erase(0, s.length());
		}
	}
	return info;
}

void draw_bar_info(adv_font* font, const game* g, const string s, int gs, int ga, const string favorites, int x, int y, int dx, int dy, const int_color& color, const string align)
{
	string info = info_get(g, gs, ga, favorites, s);

	draw_tag_info(font, info, x, y, dx, color, align);
}

void draw_menu_bar(const game* g, const string favorites, int g2, int x, int y, int dx)
{
	int_clear_alpha(x, y, dx, int_font_dy_get(), COLOR_MENU_BAR.background);

	int separator = dx > 40*int_font_dx_get() ? 1*int_font_dx_get() : 0*int_font_dx_get();
	int in_separator =  dx > 40*int_font_dx_get() ? 2*int_font_dx_get() : 1*int_font_dx_get();
	int xr = dx - separator + x;
	int xl = separator + x;

	if (g) {
		ostringstream os;
		os << setw(4) << setfill(' ') << g2;
		draw_tag_right(os.str(), xl, xr, y, in_separator, COLOR_MENU_BAR);
	}

	if (g) {
		ostringstream os;
		unsigned time;
		if (g->emulator_get()->tree_get())
			time = g->time_tree_get();
		else
			time = g->time_get();
		os << setw(3) << setfill('0') << (time/3600) << ":" << setw(2) << setfill('0') << ((time/60)%60);
		draw_tag_right(os.str(), xl, xr, y, in_separator, COLOR_MENU_BAR);
	}

	if (favorites != "") {
		ostringstream os;
		os << favorites;
		draw_tag_right(os.str(), xl, xr, y, in_separator, COLOR_MENU_BAR_TAG);
	}

	if (g && !g->type_derived_get()->undefined_get()) {
		ostringstream os;
		os << g->type_derived_get()->name_get();
		draw_tag_right(os.str(), xl, xr, y, in_separator, COLOR_MENU_BAR_TAG);
	}

	if (g) {
		ostringstream os;
		if (g->emulator_get()->tree_get())
			os << g->description_tree_get();
		else
			os << g->description_get();
		draw_tag_left(os.str(), xl, xr, y, in_separator, COLOR_MENU_BAR_TAG);
	}

	if (g) {
		ostringstream os;
		unsigned session;
		if (g->emulator_get()->tree_get())
			session = g->session_tree_get();
		else
			session = g->session_get();
		os << setw(3) << setfill(' ') << session << "p";
		draw_tag_right_whole(os.str(), xl, xr, y, in_separator, COLOR_MENU_BAR);
	}

	if (g) {
		ostringstream os;
		if (g->size_get() >= 10*1000*1000)
			os << setw(4) << g->size_get()/1000/1000 << "M";
		else
			os << setw(4) << g->size_get()/1000 << "k";
		draw_tag_right_whole(os.str(), xl, xr, y, in_separator, COLOR_MENU_BAR);
	}

	if (g && g->sizex_get() && g->sizey_get()) {
		ostringstream os;
		os << g->sizex_get() << "x" << g->sizey_get();
		draw_tag_right_whole(os.str(), xl, xr, y, in_separator, COLOR_MENU_BAR);
	}

	if (g) {
		ostringstream os;
		if (g->emulator_get()->tree_get()) {
			const game* gb = &g->clone_best_get();
			os << gb->name_get();
		} else {
			os << g->name_get();
		}
		draw_tag_left_whole(os.str(), xl, xr, y, in_separator, COLOR_MENU_BAR);
	}
}

void draw_menu_info(const game_set& gar, const game* g, int x, int y, int dx, merge_t merge, listpreview_t preview, listsort_t sort_mode, difficulty_t difficulty, bool lock)
{
	int_clear_alpha(x, y, dx, int_font_dy_get(), COLOR_MENU_BAR.background);

	int separator = dx > 40*int_font_dx_get() ? 1*int_font_dx_get() : 0*int_font_dx_get();
	int in_separator = dx > 40*int_font_dx_get() ? 2*int_font_dx_get() : 1*int_font_dx_get();
	int xr = dx - separator + x;
	int xl = separator + x;

	if (g) {
		const game* gb;
		if (g->emulator_get()->tree_get())
			gb = &g->clone_best_get();
		else
			gb = g;

		if (!gb->present_tree_get())
			draw_tag_right("MISSING", xl, xr, y, in_separator, COLOR_MENU_BAR_TAG);
		else if (gb->play_get() == play_preliminary)
			draw_tag_right("  ALPHA", xl, xr, y, in_separator, COLOR_MENU_BAR_TAG);
		else
			draw_tag_right("       ", xl, xr, y, in_separator, COLOR_MENU_BAR_TAG);
	}

	if (lock)
		draw_tag_right("locked", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN);

	switch (preview) {
	case preview_flyer: draw_tag_right("flyers", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case preview_cabinet: draw_tag_right("cabinets", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case preview_icon: draw_tag_right("icons", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case preview_marquee: draw_tag_right("marquees", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case preview_title: draw_tag_right("titles", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case preview_snap: draw_tag_right("snap", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	}

	switch (sort_mode) {
	case sort_by_name : draw_tag_right("name", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case sort_by_root_name : draw_tag_right("parent", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case sort_by_time : draw_tag_right("time", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case sort_by_session : draw_tag_right("play", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case sort_by_year : draw_tag_right("year", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case sort_by_manufacturer : draw_tag_right("manuf", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case sort_by_type : draw_tag_right("type", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case sort_by_size : draw_tag_right("size", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case sort_by_res : draw_tag_right("res", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case sort_by_info : draw_tag_right("info", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case sort_by_timepersession : draw_tag_right("timeperplay", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case sort_by_emulator : draw_tag_right("emulator", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	}

	switch (difficulty) {
	case difficulty_none : draw_tag_right("none", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case difficulty_easiest : draw_tag_right("easiest", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case difficulty_easy : draw_tag_right("easy", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case difficulty_medium : draw_tag_right("normal", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case difficulty_hard : draw_tag_right("hard", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	case difficulty_hardest : draw_tag_right("hardest", xl, xr, y, in_separator, COLOR_MENU_BAR_HIDDEN); break;
	}

	if (g) {
		if (g->info_get().length())
			draw_tag_right(g->info_get(), xl, xr, y, 0, COLOR_MENU_BAR_TAG);

		draw_tag_left(g->manufacturer_get(), xl, xr, y, 0, COLOR_MENU_BAR);

		if (g->year_get().length())
			draw_tag_left(", " + g->year_get(), xl, xr, y, 0, COLOR_MENU_BAR);

		if (g->clone_get() > 0) {
			ostringstream os;
			os << ", " << g->clone_get() << " clones";
			draw_tag_left(os.str(), xl, xr, y, 0, COLOR_MENU_BAR);
		}

		if (g->parent_get()) {
			ostringstream os;
			os << ", ";
			if (g->software_get())
				os << "software of";
			else
				os << "clone of";
			os << " " << g->parent_get()->name_get();
			draw_tag_left(os.str(), xl, xr, y, in_separator, COLOR_MENU_BAR);
		}
	}
}

struct cell_t {
	// position and size
	unsigned x;
	unsigned y;
	unsigned dx;
	unsigned dy;
};

struct cell_win_t {
	unsigned x ;
	unsigned y;
	unsigned dx;
	unsigned dy;
	listpreview_t preview;
};

void calculo_listado_5(struct cell_t* int_map, int coln, int rown, int list_x, int list_y, int list_dx, int list_dy, int name_dy, int space_x, int space_y, int diagonal)
{
	int cell_x = list_x;
	int cell_y = list_y;
	int cell_dx = (list_dx - (coln - 1) * space_x) / coln;
	int cell_dy = name_dy;
	int space_diagonal_x = abs(diagonal * (rown - 1));
	if(diagonal < 0) cell_x = cell_x + space_diagonal_x;
	
	for(int r=0;r<rown;++r) {
		for(int c=0;c<coln;++c) {
			unsigned i = r * coln + c;
			int_map[i].x = cell_x + diagonal * r + (cell_dx + space_x) * c;
			int_map[i].y = cell_y;
			int_map[i].dx = cell_dx - space_diagonal_x;
			int_map[i].dy = name_dy;
		}
		cell_y += cell_dy + space_y;
	}
}

void draw_menu_window(const game_set& gar, const menu_array& gc, struct cell_t* cell, int coln, int rown, int start, int pos, bool use_ident, merge_t merge, bool center)
{
	for(int r=0;r<rown;++r) {
		for(int c=0;c<coln;++c) {
			if (start < gc.size()) {
				if (gc[start]->has_game()) {
					const game& g = gc[start]->game_get();
					if (center)
						draw_menu_game_center(gar, g, cell->x, cell->y, cell->dx, cell->dy, start == pos, merge);
					else
						draw_menu_game_left(gar, g, cell->x, cell->y, cell->dx, start == pos, merge, use_ident ? gc[start]->ident_get() : 0);
				} else {
					draw_menu_desc(gc[start]->desc_get(), cell->x, cell->y, cell->dx, start == pos);
				}
			} else {
				draw_menu_empty(cell->x, cell->y, cell->dx, cell->dy, start == pos);
			}
			++start;
			++cell;
		}
	}
}

void draw_list_window(const game_set& gar, const menu_array& gc, struct cell_t* cell, int coln, int rown, int start, int pos, bool use_ident, merge_t merge, const string align)
{
	for(int r=0;r<rown;++r) {
		for(int c=0;c<coln;++c) {
			if (start < gc.size()) {
				if (gc[start]->has_game()) {
					const game& g = gc[start]->game_get();
					draw_list_game(g, cell->x, cell->y, cell->dx, start == pos, align);
				} else {
					draw_menu_desc(gc[start]->desc_get(), cell->x, cell->y, cell->dx, start == pos);
				}
			} else {
				draw_menu_empty(cell->x, cell->y, cell->dx, cell->dy, start == pos);
			}
			++start;
			++cell;
		}
	}
}

void draw_menu_right(int x, int y, int dx, int dy, int pos, int delta, int max)
{
	if (max <= 1)
		return;

	if (pos >= max)
		pos = max - 1;
	if (pos + delta > max)
		delta = max - pos;

	int y0 = pos * (dy-dx-1) / (max-1);
	int y1 = dx + (pos+delta-1) * (dy-dx-1) / (max-1);

	int_clear_alpha(x, y, dx, dy, COLOR_MENU_GRID.background);
	int_clear_alpha(x, y+y0, dx, y1-y0+1, COLOR_MENU_GRID.foreground);
}

void draw_menu_desc(const string& desc, unsigned counter)
{
	int border = int_font_dx_get()/2;

	int dx = int_put_width(desc);
	int x = (int_dx_get() - dx) / 2;

	int dy = int_font_dy_get();
	int y;

	if (counter % 2 == 0)
		y = int_dy_get() - 3 * int_font_dy_get();
	else
		y = 2 * int_font_dy_get();

	int_box(x-border, y-border, dx+2*border, dy+border*2, 1, COLOR_CHOICE_NORMAL.foreground);
	int_clear(x-border+1, y-border+1, dx+2*border-2, dy+border*2-2, COLOR_CHOICE_NORMAL.background);

	int_put(x, y, dx, desc, COLOR_CHOICE_TITLE);
}

// ------------------------------------------------------------------------
// Menu utility

bool menu_fast_compare(const string& game, const string& fast)
{
	unsigned igame = 0;
	unsigned ifast = 0;
	while (igame < game.length() && ifast < fast.length()) {
		while (igame < game.length() && !isalnum(game[igame]))
		++igame;
		if (igame < game.length()) {
			if (toupper(game[igame]) != toupper(fast[ifast]))
				return false;
			++igame;
			++ifast;
		}
	}
	return ifast == fast.length();
}

//--------------------------------------------------------------------------
// Backdrop

bool sound_find_preview(resource& path, const config_state& rs, const game* pgame)
{
	(void)rs;
	if (pgame && pgame->preview_find(path, &game::preview_sound_get))
		return true;
	return false;
}

bool backdrop_find_preview_strict(resource& path, listpreview_t preview, const game* pgame, bool only_clip)
{
	if (pgame) {
		switch (preview) {
		case preview_icon :
			if (pgame->preview_find(path, &game::preview_icon_get))
				return true;
			break;
		case preview_marquee :
			if (pgame->preview_find(path, &game::preview_marquee_get))
				return true;
			break;
		case preview_title :
			if (pgame->preview_find(path, &game::preview_title_get))
				return true;
			break;
		case preview_snap :
			if (pgame->preview_find(path, &game::preview_clip_get))
				return true;
			if (!only_clip) {
				if (pgame->preview_find(path, &game::preview_snap_get))
					return true;
			}
			break;
		case preview_flyer :
			if (pgame->preview_find(path, &game::preview_flyer_get))
				return true;
			break;
		case preview_cabinet :
			if (pgame->preview_find(path, &game::preview_cabinet_get))
				return true;
			break;
		}
	}

	return false;
}

bool backdrop_find_preview_default(resource& path, unsigned& aspectx, unsigned& aspecty, listpreview_t preview, const game* pgame, const config_state& rs)
{
	if (backdrop_find_preview_strict(path, preview, pgame, false))
		return true;

	path = resource();

	// reset the aspect, the default preview may be anything
	aspectx = 0;
	aspecty = 0;

	switch (preview) {
	case preview_icon :
		if (rs.preview_default_icon != "none")
			path = rs.preview_default_icon;
		break;
	case preview_marquee :
		if (rs.preview_default_marquee != "none")
			path = rs.preview_default_marquee;
		break;
	case preview_title :
		if (rs.preview_default_title != "none")
			path = rs.preview_default_title;
		break;
	case preview_snap :
		if (rs.preview_default_snap != "none")
			path = rs.preview_default_snap;
		break;
	case preview_flyer :
		if (rs.preview_default_flyer != "none")
			path = rs.preview_default_flyer;
		break;
	case preview_cabinet :
		if (rs.preview_default_cabinet != "none")
			path = rs.preview_default_cabinet;
		break;
	}

	if (!path.is_valid() && rs.preview_default != "none")
		path = rs.preview_default;

	return path.is_valid();
}

void backdrop_game_set(const game* effective_game, unsigned back_pos, listpreview_t preview, bool current, bool highlight, bool clip, config_state& rs)
{
	resource backdrop_res;
	resource clip_res;

	unsigned aspectx;
	unsigned aspecty;
	if (effective_game && (preview == preview_snap || preview == preview_title)) {
		aspectx = effective_game->aspectx_get();
		aspecty = effective_game->aspecty_get();

		if (clip && preview == preview_snap) {
			if (rs.clip_mode != clip_none && effective_game->preview_find(clip_res, &game::preview_clip_get)) {
				int_clip_set(back_pos, clip_res, aspectx, aspecty, current);
			} else {
				int_clip_clear(back_pos);
			}
		} else {
			int_clip_clear(back_pos);
		}
	} else {
		aspectx = 0;
		aspecty = 0;

		int_clip_clear(back_pos);
	}

	if (backdrop_find_preview_default(backdrop_res, aspectx, aspecty, preview, effective_game, rs)) {
		int_backdrop_set(back_pos, backdrop_res, highlight, aspectx, aspecty);
		if (current)
			rs.current_backdrop = backdrop_res;
	} else {
		int_backdrop_clear(back_pos, highlight);
		if (current)
			rs.current_backdrop = resource();
	}
}

void backdrop_index_set(unsigned pos, menu_array& gc, unsigned back_pos, listpreview_t preview, bool current, bool highlight, bool clip, config_state& rs)
{
	const game* effective_game;

	if (pos < gc.size() && gc[pos]->has_game())
		effective_game = &gc[pos]->game_get().clone_best_get();
	else
		effective_game = 0;

	backdrop_game_set(effective_game, back_pos, preview, current, highlight, clip, rs);
}

//--------------------------------------------------------------------------
// Menu run

static void run_background_reset(config_state& rs)
{
	play_background_stop(PLAY_PRIORITY_GAME_BACKGROUND);
	rs.current_sound = resource();
}

static void run_background_set(config_state& rs, const resource& sound)
{
	if (sound.is_valid()) {
		log_std(("menu: play game music '%s'\n", sound.path_get().c_str()));
		play_background_stop(PLAY_PRIORITY_BACKGROUND);
		play_background_effect(sound, PLAY_PRIORITY_GAME_BACKGROUND, false);
		rs.current_sound = sound;
	} else {
		log_std(("menu: no game music found\n"));
	}
}

static void run_background_wait(config_state& rs, const resource& sound, bool silent, int pos_current, int pos_max)
{
	target_clock_t back_start = target_clock();

	// delay before the background music start
	target_clock_t back_stop = back_start + rs.repeat_rep * TARGET_CLOCKS_PER_SEC / 666; // / 666 = * 3 / 2 / 1000

	// short busy wait (int_event_waiting contains a idle call) */
	while (!int_event_waiting() && (target_clock() < back_stop)) {
	}

	if (!int_event_waiting()) {
		bool sound_done = false; // game sound terminated
		bool clip_done = false; // clip sound terminate

		if (!silent) {
			// stop the previous playing
			run_background_reset(rs);

			// start the new game play
			run_background_set(rs, sound);

			// start the new game clip
			int_clip_start(pos_current);
		}

		// wait until something pressed
		while (!int_event_waiting()) {
			// start some background music if required

			if (!play_background_is_active()) {
				sound_done = true;
				rs.current_sound = resource();
				if (!rs.sound_background.empty()) {
					string path = file_select_random(rs.sound_background);
					log_std(("menu: play background music '%s'\n", path.c_str()));
					play_background_effect(path, PLAY_PRIORITY_BACKGROUND, false);
				} else if (rs.sound_background_loop.length()) {
					log_std(("menu: play background effect '%s'\n", rs.sound_background_loop.c_str()));
					play_background_effect(rs.sound_background_loop, PLAY_PRIORITY_BACKGROUND, true);
				} else {
					log_std(("menu: no background effect\n"));
				}
			}

			if (rs.clip_mode == clip_singleloop || rs.clip_mode == clip_multiloop) {
				if (!int_clip_is_active(pos_current)) {
					int_clip_start(pos_current);
				}
				clip_done = true; // disable the control, the clip never finish
			} else if (rs.clip_mode == clip_multiloopall) {
				unsigned i;
				for(i=0;i<pos_max;++i) {
					if (!int_clip_is_active(i)) {
						int_clip_start(i);
					}
				}
				clip_done = true; // disable the control, the clip never finish
			} else {
				if (!int_clip_is_active(pos_current)) {
					clip_done = true;
				}
			}
		}
	}
}

void backdrop_win_sort(int mac, struct cell_win_t* back) {
	int i, j;
	cell_win_t aux;
		
	for (i=0; i<mac-1; i++) {
		for (j=0; j<mac-1; j++) {
			if (size_win_sort(back[j].x, back[j].y, back[j].dx, back[j].dy) > size_win_sort(back[j+1].x, back[j+1].y, back[j+1].dx, back[j+1].dy)) {
				aux = back[j];
				back[j] = back[j+1];
				back[j+1] = aux;
			}
		}
	}
}

void comprobar_medidas(unsigned& x, unsigned& y, unsigned& dx, unsigned& dy) {
	unsigned resolution_x = int_dx_get();
	unsigned resolution_y = int_dy_get();
	if ( x+dx > resolution_x || y+dy > resolution_y) {
		dy = 0;
	}
}

int orientation_load(const string& ori) {
	string s = ori;
	int i;
	i = 0;
	int mask = 0;
	while (i<s.length()) {
		string arg = arg_get(s, i);
		if (arg == "flip_xy")
			mask ^= ADV_ORIENTATION_FLIP_XY;
		else if (arg == "mirror_x")
			mask ^= ADV_ORIENTATION_FLIP_X;
		else if (arg == "mirror_y")
			mask ^= ADV_ORIENTATION_FLIP_Y;
		else {
			//config_error_la("display_orientation " + s, arg);
			//return false;
		}
	}

	int_enable_orientation(mask);
	
	return mask;
}

void disable_fonts() {
	if(int_font_menu)	int_disable_font_info(int_font_menu);
	if(int_font_list)	int_disable_font_info(int_font_list);
	if(int_font_info_1)	int_disable_font_info(int_font_info_1);
	if(int_font_info_2)	int_disable_font_info(int_font_info_2);
	if(int_font_info_3)	int_disable_font_info(int_font_info_3);
	if(int_font_info_4)	int_disable_font_info(int_font_info_4);
	if(int_font_info_5)	int_disable_font_info(int_font_info_5);
}

void color_default_load() {
	COLOR_HELP_NORMAL = DCOLOR_HELP_NORMAL;
	COLOR_HELP_TAG = DCOLOR_HELP_TAG;
	COLOR_CHOICE_TITLE = DCOLOR_CHOICE_TITLE;
	COLOR_CHOICE_NORMAL = DCOLOR_CHOICE_NORMAL;
	COLOR_CHOICE_SELECT = DCOLOR_CHOICE_SELECT;
	COLOR_CHOICE_HIDDEN = DCOLOR_CHOICE_HIDDEN;
	COLOR_CHOICE_HIDDEN_SELECT = DCOLOR_CHOICE_HIDDEN_SELECT;
	COLOR_MENU_NORMAL = DCOLOR_MENU_NORMAL;
	COLOR_MENU_HIDDEN = DCOLOR_MENU_HIDDEN;
	COLOR_MENU_TAG = DCOLOR_MENU_TAG;
	COLOR_MENU_SELECT = DCOLOR_MENU_SELECT;
	COLOR_MENU_HIDDEN_SELECT = DCOLOR_MENU_HIDDEN_SELECT;
	COLOR_MENU_TAG_SELECT = DCOLOR_MENU_TAG_SELECT;
	COLOR_MENU_BAR = DCOLOR_MENU_BAR;
	COLOR_MENU_BAR_TAG = DCOLOR_MENU_BAR_TAG;
	COLOR_MENU_BAR_HIDDEN = DCOLOR_MENU_BAR_HIDDEN;
	COLOR_MENU_GRID = DCOLOR_MENU_GRID;
	COLOR_MENU_BACKDROP = DCOLOR_MENU_BACKDROP;
	COLOR_MENU_ICON = DCOLOR_MENU_ICON;
	COLOR_MENU_CURSOR = DCOLOR_MENU_CURSOR;
}

void color_rc_load() {
	COLOR_HELP_NORMAL = RCOLOR_HELP_NORMAL;
	COLOR_HELP_TAG = RCOLOR_HELP_TAG;
	COLOR_CHOICE_TITLE = RCOLOR_CHOICE_TITLE;
	COLOR_CHOICE_NORMAL = RCOLOR_CHOICE_NORMAL;
	COLOR_CHOICE_SELECT = RCOLOR_CHOICE_SELECT;
	COLOR_CHOICE_HIDDEN = RCOLOR_CHOICE_HIDDEN;
	COLOR_CHOICE_HIDDEN_SELECT = RCOLOR_CHOICE_HIDDEN_SELECT;
	COLOR_MENU_NORMAL = RCOLOR_MENU_NORMAL;
	COLOR_MENU_HIDDEN = RCOLOR_MENU_HIDDEN;
	COLOR_MENU_TAG = RCOLOR_MENU_TAG;
	COLOR_MENU_SELECT = RCOLOR_MENU_SELECT;
	COLOR_MENU_HIDDEN_SELECT = RCOLOR_MENU_HIDDEN_SELECT;
	COLOR_MENU_TAG_SELECT = RCOLOR_MENU_TAG_SELECT;
	COLOR_MENU_BAR = RCOLOR_MENU_BAR;
	COLOR_MENU_BAR_TAG = RCOLOR_MENU_BAR_TAG;
	COLOR_MENU_BAR_HIDDEN = RCOLOR_MENU_BAR_HIDDEN;
	COLOR_MENU_GRID = RCOLOR_MENU_GRID;
	COLOR_MENU_BACKDROP = RCOLOR_MENU_BACKDROP;
	COLOR_MENU_ICON = RCOLOR_MENU_ICON;
	COLOR_MENU_CURSOR = RCOLOR_MENU_CURSOR;
}

static int run_menu_user(config_state& rs, bool flipxy, menu_array& gc, sort_item_func* category_extract, bool silent, string over_msg)
{
	int coln; // number of columns
	int rown; // number of rows

	int scr_x; // useable screen
	int scr_y;
	int scr_dx;
	int scr_dy;

	int win_x; // menu window
	int win_y;
	int win_dx;
	int win_dy;

	int backdrop_x = 0; // backdrop window (if one)
	int backdrop_y = 0;
	int backdrop_dx = 0;
	int backdrop_dy = 0;

	int bar_top_x; // top bar
	int bar_top_y;
	int bar_top_dx;
	int bar_top_dy;

	int bar_bottom_x; // bottom bar
	int bar_bottom_y;
	int bar_bottom_dx;
	int bar_bottom_dy;

	int bar_left_x; // left bar
	int bar_left_y;
	int bar_left_dx;
	int bar_left_dy;

	int bar_right_x; // right bar
	int bar_right_y;
	int bar_right_dx;
	int bar_right_dy;

	int box_x = 0;
	int box_y = 0;
	int box_dx = 0;
	int box_dy = 0;
	adv_bool box;

	int space_x; // space between tiles
	int space_y;

	int name_dy;

	unsigned backdrop_mac = 0; // number of backdrops
	struct cell_t* backdrop_map; // map of the backdrop positions
	struct cell_t* backdrop_map_bis; // alternate map of the backdrop positions

	unsigned backdrop_win_mac = 0;
	struct cell_win_t* backdrop_win = 0;
	bool backdrop_win_border = true;
	
	unsigned game_count; // counter of game in the container
	unsigned game_selected;

	unsigned ui_right = 0; // user limit
	unsigned ui_left = 0;
	unsigned ui_top = 0;
	unsigned ui_bottom = 0;
	
	string img_background;
	string font_path;

	unsigned ui_list_x = video_size_x()/10, ui_list_y = video_size_y()/10, ui_list_dx = video_size_x()/5, ui_list_dy = video_size_y()/3;
	unsigned ui_grid_x = 0, ui_grid_y = 0, ui_grid_dx = 0, ui_grid_dy = 0;

	unsigned bar_info_1_x = 0, bar_info_1_y = 0, bar_info_1_dx = 0, bar_info_1_dy = 0;
	string bar_info_1_text = "", bar_info_1_font_path="", bar_info_1_align="left";
	int bar_info_1_font_dx = 0;
	unsigned bar_info_2_x = 0, bar_info_2_y = 0, bar_info_2_dx = 0, bar_info_2_dy = 0;
	string bar_info_2_text = "", bar_info_2_font_path="", bar_info_2_align="left";
	int bar_info_2_font_dx= 0;
	unsigned bar_info_3_x = 0, bar_info_3_y = 0, bar_info_3_dx = 0, bar_info_3_dy = 0;
	string bar_info_3_text = "", bar_info_3_font_path="", bar_info_3_align="left";
	int bar_info_3_font_dx = 0;
	unsigned bar_info_4_x = 0, bar_info_4_y = 0, bar_info_4_dx = 0, bar_info_4_dy = 0;
	string bar_info_4_text = "", bar_info_4_font_path="", bar_info_4_align="left";
	int bar_info_4_font_dx = 0;
	unsigned bar_info_5_x = 0, bar_info_5_y = 0, bar_info_5_dx = 0, bar_info_5_dy = 0;
	string bar_info_5_text = "", bar_info_5_font_path="", bar_info_5_align="left";
	int bar_info_5_font_dx = 0;

	unsigned cols = 1; // numero de columnas de juegos en la lista (1 to ...)
	unsigned space_cols = 0; // espacio entre columnas
	string rows = "auto"; // numero de juegos en la lista de juegos ("auto" | 1 to ...)
	string space_rows = "auto"; // espacio entre filas
	int diagonal = 0; // listado: transformacion - diagonal
	string list_align = "left";
	
	unsigned translucency = 0;
	bool imagen_fondo_cargada = false;

	unsigned fontsize_X = 0;
	unsigned fontsize_Y = 0;
	string menu_font_path = "none";
	unsigned menu_fontsize_X= 0;
	unsigned menu_fontsize_Y = 0;
	bool frenado = false;
	int rel = 0;
	int row_rel = 0;
	int col_rel = 0;
	
	string emu_start = "none";
	int video_orientation = 0;
	
	log_std(("menu: user begin\n"));

	for(pemulator_container::iterator j = rs.emu.begin();j!=rs.emu.end();j++) {
		if ((*j)->state_get() == 1) {
			if(rs.mode_get() != mode_custom) {
				video_orientation = rs.video_orientation_effective;
				
				emu_start = (*j)->nocustom_start_path_get();
				if (emu_start == "")
					emu_start = "none";
				
				img_background = (*j)->nocustom_background_path_get();
				if (img_background == "" || img_background == "default")
					img_background = rs.ui_back;
				
				font_path = (*j)->nocustom_font_path_get();
				if (font_path == "" || font_path == "default")
					font_path = rs.video_font_path;

				string fontsize = (*j)->nocustom_font_size_get();
				if (fontsize == "") {
					fontsize_X= rs.video_fontx;
					fontsize_Y = rs.video_fonty;
				} else {
					string a0, a1;
					if (config_split_custom(fontsize, a0, a1)) {
						fontsize_Y= atoi(a0.c_str());
						fontsize_X = atoi(a1.c_str());
					}
				}

				if (fontsize_Y >= 5 && fontsize_Y <= 100)
					fontsize_Y = video_size_y() / fontsize_Y;
				else
					fontsize_Y = video_size_y() / 45;
				if (fontsize_X >= 5 && fontsize_X <= 200)
					fontsize_X = video_size_x() / fontsize_X;
				else
					fontsize_X = fontsize_Y * video_size_x() * 3 / video_size_y() / 4;
				
				translucency = rs.ui_translucency;

				color_rc_load();
				
				string c_font = (*j)->nocustom_font_color_get();
				if(color_nocustom(COLOR_MENU_NORMAL, c_font, false, video_color_def(), translucency)) {
					COLOR_HELP_NORMAL = COLOR_HELP_TAG = COLOR_CHOICE_TITLE = COLOR_CHOICE_NORMAL = COLOR_MENU_NORMAL;
					COLOR_CHOICE_HIDDEN = COLOR_MENU_HIDDEN = COLOR_MENU_TAG = COLOR_MENU_NORMAL;
					COLOR_MENU_BAR_TAG = COLOR_MENU_BAR_HIDDEN = COLOR_MENU_GRID = COLOR_MENU_NORMAL;
					COLOR_MENU_BACKDROP = COLOR_MENU_ICON = COLOR_MENU_CURSOR = COLOR_MENU_NORMAL;
					COLOR_MENU_BAR = COLOR_MENU_NORMAL;

					string c_font_select = (*j)->nocustom_font_color_select_get();
					if(c_font_select == "")
						invertir_colores(c_font_select, c_font);
					
					if(color_nocustom(COLOR_MENU_SELECT, c_font_select, true, video_color_def(), translucency))
						COLOR_MENU_TAG_SELECT = COLOR_CHOICE_SELECT = COLOR_CHOICE_HIDDEN_SELECT = COLOR_MENU_HIDDEN_SELECT = COLOR_MENU_SELECT;
				}
				
				ui_right = rs.ui_right;
				ui_left = rs.ui_left;
				ui_top = rs.ui_top;
				ui_bottom = rs.ui_bottom;

			} else {
				string orientation = (*j)->custom_video_orientation_get();
				video_orientation = orientation_load(orientation);
				flipxy = (video_orientation & ADV_ORIENTATION_FLIP_XY) != 0;
				
				bool flag_special = false; 

				color_default_load();

				emu_start = (*j)->custom_start_path_get();
				if (emu_start == "")
					emu_start = "none";
				
				img_background = (*j)->custom_background_path_get();
				if (img_background == "")
					img_background = "none";

				string c_background = (*j)->custom_background_color_get();
				if(!color_custom(LCOLOR_BACKGROUND, c_background, flag_special, video_color_def())) {
					LCOLOR_BACKGROUND = DCOLOR_BACKGROUND;
				} else {
					
				}

				menu_font_path = (*j)->custom_menu_font_path_get();
				if(menu_font_path == "")
					menu_font_path = "none";
				
				string menu_fontsize = (*j)->custom_menu_font_size_get();
				if (menu_fontsize != "") {
					string a0, a1;
					if (config_split_custom(menu_fontsize, a0, a1)) {
						menu_fontsize_Y= atoi(a0.c_str());
						menu_fontsize_X = atoi(a1.c_str());
					}
				}

				string c_font_menu_title = (*j)->custom_menu_title_color_get();
				if(color_custom(LCOLOR_FONT_MENU_TITLE, c_font_menu_title, flag_special, video_color_def())) {
					COLOR_CHOICE_TITLE = LCOLOR_FONT_MENU_TITLE;
				}
				string c_font_menu = (*j)->custom_menu_font_color_get();
				if(color_custom(LCOLOR_FONT_MENU, c_font_menu, flag_special, video_color_def())) {
					COLOR_CHOICE_NORMAL = COLOR_CHOICE_HIDDEN = LCOLOR_FONT_MENU;
				}
				string c_font_menu_select = (*j)->custom_menu_font_select_color_get();
				if(color_custom(LCOLOR_FONT_MENU_SELECT, c_font_menu_select, flag_special, video_color_def())) {
					COLOR_CHOICE_SELECT = COLOR_CHOICE_HIDDEN_SELECT = LCOLOR_FONT_MENU_SELECT;
				}
				
				font_path = (*j)->custom_font_path_get();
				if (font_path == "")
					font_path = "none";

				string fontsize = (*j)->custom_font_size_get();
				if (fontsize != "") {
					string a0="", a1="";
					if (config_split_custom(fontsize, a0, a1)) {
						fontsize_Y= atoi(a0.c_str());
						fontsize_X = atoi(a1.c_str());
					}
				}

				string list_pos_size = (*j)->custom_list_pos_size_get();
				if (list_pos_size != "" && list_pos_size != "none") {
					string a0, a1, a2, a3;
					if (config_split_custom(list_pos_size, a0, a1, a2, a3)) {
						ui_list_x = atoi(a0.c_str());
						ui_list_y = atoi(a1.c_str());
						ui_list_dx = atoi(a2.c_str());
						ui_list_dy = atoi(a3.c_str());
					}
				}

				string list_cols = (*j)->custom_list_cols_get();
				if (list_cols != "" && list_cols != "default") {
					string a0, a1;
					if (config_split_custom(list_cols, a0, a1)) {
						cols = atoi(a0.c_str());
						space_cols = atoi(a1.c_str());
					}
				}
				
				string list_rows = (*j)->custom_list_rows_get();
				if (list_rows != "" && list_rows != "auto") {
					string a0, a1;
					if (config_split_custom(list_rows, a0, a1)) {
						rows = a0;
						if (a1 == "")
							a1 = "auto";
						space_rows = a1;
					}
				}
				
				string list_diagonal = (*j)->custom_list_diagonal_get();
				if (list_diagonal != "" && list_diagonal != "none")
					diagonal = atoi(list_diagonal.c_str());
					
				list_align = (*j)->custom_list_align_get();
				if(list_align == "") 
					list_align == "left";
				
				string c_font = (*j)->custom_color_font_get();
				if(color_custom(LCOLOR_FONT_LIST, c_font, flag_special, video_color_def())) {
					COLOR_MENU_NORMAL = COLOR_MENU_TAG = COLOR_MENU_HIDDEN = LCOLOR_FONT_LIST;
				}
				string c_font_select = (*j)->custom_color_font_select_get();
				if(color_custom(LCOLOR_FONT_LIST_SELECT, c_font_select, flag_special, video_color_def())) {
					COLOR_MENU_SELECT = COLOR_MENU_TAG_SELECT = COLOR_MENU_HIDDEN_SELECT = LCOLOR_FONT_LIST_SELECT;
				}

				string selected_rel = (*j)->custom_list_selected_pos_get();
				if (selected_rel != "" && selected_rel != "none") {
					string a0, a1;
					if (config_split_custom(selected_rel, a0, a1)) {
						row_rel = atoi(a0.c_str()) - 1;
						col_rel = atoi(a1.c_str()) - 1;
						if (col_rel < 0)
							col_rel = 0;
						if (row_rel >= 0)
							frenado = true;
					}
				}

				string grid_pos_size = (*j)->custom_scroll_pos_size_get();
				if (grid_pos_size != "" && grid_pos_size != "none") {
					string a0, a1, a2, a3;
					if (config_split_custom(grid_pos_size, a0, a1, a2, a3)) {
						ui_grid_x = atoi(a0.c_str());
						ui_grid_y = atoi(a1.c_str());
						ui_grid_dx = atoi(a2.c_str());
						ui_grid_dy = atoi(a3.c_str());
					}
				}
				
				string c_grid = (*j)->custom_color_scroll_get();
				if(color_custom(LCOLOR_GRID, c_grid, flag_special, video_color_def())) {
					COLOR_MENU_GRID = LCOLOR_GRID;
				}
				
				string c_win = (*j)->custom_win_color_get();
				if(!color_custom(LCOLOR_WIN, c_win, backdrop_win_border, video_color_def())) 
					backdrop_win_border = false;

 				backdrop_win = new cell_win_t[6];

				unsigned mac = 0;
				string win_snaps_pos_size = (*j)->custom_win_snaps_get(); 
				if (win_snaps_pos_size != "" && win_snaps_pos_size != "none") {
					string a0, a1, a2, a3;
					if (config_split_custom(win_snaps_pos_size, a0, a1, a2, a3)) {
						if(a3 != "0") {
							backdrop_win[mac].x = atoi(a0.c_str());
							backdrop_win[mac].y = atoi(a1.c_str());
							backdrop_win[mac].dx = atoi(a2.c_str());
							backdrop_win[mac].dy = atoi(a3.c_str());
							backdrop_win[mac].preview = preview_snap;
							mac++;
						}
					}
				}
				string win_flyers_pos_size = (*j)->custom_win_flyers_get();
				if (win_flyers_pos_size != "" && win_flyers_pos_size != "none") {
					string a0, a1, a2, a3;
					if (config_split_custom(win_flyers_pos_size, a0, a1, a2, a3)) {
						if(a3 != "0") {
							backdrop_win[mac].x = atoi(a0.c_str());
							backdrop_win[mac].y = atoi(a1.c_str());
							backdrop_win[mac].dx = atoi(a2.c_str());
							backdrop_win[mac].dy = atoi(a3.c_str());
							backdrop_win[mac].preview = preview_flyer;
							mac++;
						}
					}
				}
				string win_cabinets_pos_size = (*j)->custom_win_cabinets_get();
				if (win_cabinets_pos_size != "" && win_cabinets_pos_size != "none") {
					string a0, a1, a2, a3;
					if (config_split_custom(win_cabinets_pos_size, a0, a1, a2, a3)) {
						if(a3 != "0") {
							backdrop_win[mac].x = atoi(a0.c_str());
							backdrop_win[mac].y = atoi(a1.c_str());
							backdrop_win[mac].dx = atoi(a2.c_str());
							backdrop_win[mac].dy = atoi(a3.c_str());
							backdrop_win[mac].preview = preview_cabinet;
							mac++;
						}
					}
				}
				string win_icons_pos_size = (*j)->custom_win_icons_get(); 
				if (win_icons_pos_size != "" && win_icons_pos_size != "none") {
					string a0, a1, a2, a3;
					if (config_split_custom(win_icons_pos_size, a0, a1, a2, a3)) {
						if(a3 != "0") {
							backdrop_win[mac].x = atoi(a0.c_str());
							backdrop_win[mac].y = atoi(a1.c_str());
							backdrop_win[mac].dx = atoi(a2.c_str());
							backdrop_win[mac].dy = atoi(a3.c_str());
							backdrop_win[mac].preview = preview_icon;
							mac++;
						}
					}
				}
				string win_marquees_pos_size = (*j)->custom_win_marquees_get();
				if (win_marquees_pos_size != "" && win_marquees_pos_size != "none") {
					string a0, a1, a2, a3;
					if (config_split_custom(win_marquees_pos_size, a0, a1, a2, a3)) {
						if(a3 != "0") {
							backdrop_win[mac].x = atoi(a0.c_str());
							backdrop_win[mac].y = atoi(a1.c_str());
							backdrop_win[mac].dx = atoi(a2.c_str());
							backdrop_win[mac].dy = atoi(a3.c_str());
							backdrop_win[mac].preview = preview_marquee;
							mac++;
						}
					}
				}
				string win_titles_pos_size = (*j)->custom_win_titles_get();
				if (win_titles_pos_size != "" && win_titles_pos_size != "none") {
					string a0, a1, a2, a3;
					if (config_split_custom(win_titles_pos_size, a0, a1, a2, a3)) {
						if(a3 != "0") {
							backdrop_win[mac].x = atoi(a0.c_str());
							backdrop_win[mac].y = atoi(a1.c_str());
							backdrop_win[mac].dx = atoi(a2.c_str());
							backdrop_win[mac].dy = atoi(a3.c_str());
							backdrop_win[mac].preview = preview_title;
							mac++;
						}
					}
				}
	
				backdrop_win_mac = mac;

				string bar_info_1_pos_size = (*j)->custom_bar_info_1_get();
				if (bar_info_1_pos_size != "" && bar_info_1_pos_size != "none") {
					string a0, a1, a2, a3;
					if (config_split_custom(bar_info_1_pos_size, a0, a1, a2, a3)) {
						bar_info_1_x = atoi(a0.c_str());
						bar_info_1_y = atoi(a1.c_str());
						bar_info_1_dx = atoi(a2.c_str());
						bar_info_1_dy = atoi(a3.c_str());
					}
					
					bar_info_1_font_path = (*j)->custom_bar_info_1_font_get();
					if(bar_info_1_font_path == "")
						bar_info_1_font_path = "none";
					
					bar_info_1_text = (*j)->custom_bar_info_1_text_get();

					bar_info_1_align = (*j)->custom_bar_info_1_align_get();
					if (bar_info_1_align == "")
						bar_info_1_align = "left";
					
					string c_info = (*j)->custom_bar_info_1_color_get();
					if(!color_custom(LCOLOR_BAR_INFO_A, c_info, flag_special, video_color_def()))
						LCOLOR_BAR_INFO_A = DCOLOR_MENU_NORMAL;
				}
				string bar_info_2_pos_size = (*j)->custom_bar_info_2_get();
				if (bar_info_2_pos_size != "" && bar_info_2_pos_size != "none") {
					string a0, a1, a2, a3; 
					if (config_split_custom(bar_info_2_pos_size, a0, a1, a2, a3)) {
						bar_info_2_x = atoi(a0.c_str());
						bar_info_2_y = atoi(a1.c_str());
						bar_info_2_dx = atoi(a2.c_str());
						bar_info_2_dy = atoi(a3.c_str());
					}

					bar_info_2_font_path = (*j)->custom_bar_info_2_font_get();
					if(bar_info_2_font_path == "")
						bar_info_2_font_path = "none";
					
					bar_info_2_text = (*j)->custom_bar_info_2_text_get();

					bar_info_2_align = (*j)->custom_bar_info_2_align_get();
					if (bar_info_2_align == "")
						bar_info_2_align = "left";
					
					string c_info = (*j)->custom_bar_info_2_color_get();
					if(!color_custom(LCOLOR_BAR_INFO_B, c_info,flag_special, video_color_def()))
						LCOLOR_BAR_INFO_B = DCOLOR_MENU_NORMAL;
				}
				string bar_info_3_pos_size = (*j)->custom_bar_info_3_get();
				if (bar_info_3_pos_size != "" && bar_info_3_pos_size != "none") {
					string a0, a1, a2, a3;
					if (config_split_custom(bar_info_3_pos_size, a0, a1, a2, a3)) {
						bar_info_3_x = atoi(a0.c_str());
						bar_info_3_y = atoi(a1.c_str());
						bar_info_3_dx = atoi(a2.c_str());
						bar_info_3_dy = atoi(a3.c_str());
					}

					bar_info_3_font_path = (*j)->custom_bar_info_3_font_get();
					if(bar_info_3_font_path == "")
						bar_info_3_font_path = "none";

					bar_info_3_text = (*j)->custom_bar_info_3_text_get();

					bar_info_3_align = (*j)->custom_bar_info_3_align_get();
					if (bar_info_3_align == "")
						bar_info_3_align = "left";
					
					string c_info = (*j)->custom_bar_info_3_color_get();
					if(!color_custom(LCOLOR_BAR_INFO_C, c_info, flag_special, video_color_def()))
						LCOLOR_BAR_INFO_C = DCOLOR_MENU_NORMAL;
				}
				string bar_info_4_pos_size = (*j)->custom_bar_info_4_get();
				if (bar_info_4_pos_size != "" && bar_info_4_pos_size != "none") {
					string a0, a1, a2, a3;
					if (config_split_custom(bar_info_4_pos_size, a0, a1, a2, a3)) {
						bar_info_4_x = atoi(a0.c_str());
						bar_info_4_y = atoi(a1.c_str());
						bar_info_4_dx = atoi(a2.c_str());
						bar_info_4_dy = atoi(a3.c_str());
					}

					bar_info_4_font_path = (*j)->custom_bar_info_4_font_get();
					if(bar_info_4_font_path == "")
						bar_info_4_font_path = "none";

					bar_info_4_text = (*j)->custom_bar_info_4_text_get();

					bar_info_4_align = (*j)->custom_bar_info_4_align_get();
					if (bar_info_4_align == "")
						bar_info_4_align = "left";
					
					string c_info = (*j)->custom_bar_info_4_color_get();
					if(!color_custom(LCOLOR_BAR_INFO_D, c_info, flag_special, video_color_def()))
						LCOLOR_BAR_INFO_D = DCOLOR_MENU_NORMAL;
				}
				
				string bar_info_5_pos_size = (*j)->custom_bar_info_5_get();
				if (bar_info_5_pos_size != "" && bar_info_5_pos_size != "none") {
					string a0, a1, a2, a3;
					if (config_split_custom(bar_info_5_pos_size, a0, a1, a2, a3)) {
						bar_info_5_x = atoi(a0.c_str());
						bar_info_5_y = atoi(a1.c_str());
						bar_info_5_dx = atoi(a2.c_str());
						bar_info_5_dy = atoi(a3.c_str());
					}

					bar_info_5_font_path = (*j)->custom_bar_info_5_font_get();
					if(bar_info_5_font_path == "")
						bar_info_5_font_path = "none";

					bar_info_5_text = (*j)->custom_bar_info_5_text_get();

					bar_info_5_align = (*j)->custom_bar_info_5_align_get();
					if (bar_info_5_align == "")
						bar_info_5_align = "left";
					
					string c_info = (*j)->custom_bar_info_5_color_get();
					if(!color_custom(LCOLOR_BAR_INFO_E, c_info, flag_special, video_color_def()))
						LCOLOR_BAR_INFO_E = DCOLOR_MENU_NORMAL;
				}
				
				ui_right = 0;
				ui_left = 0;
				ui_top = 0;
				ui_bottom = 0;
				
			}
			break;
		}
	}

	//arregla bug: pantallazos video start emu
	if (!is_loaded && emu_start != "none") {
		int_clip_start_emu(emu_start, false);
	} else {
		int_clear(COLOR_MENU_GRID.background);
	}
	
	// load the background image
	if (img_background != "none") {
		unsigned scale_x, scale_y;
		
		if (int_image(img_background, scale_x, scale_y)) {
			// scale the user limit
			if (scale_x && scale_y) {
				ui_right = rs.ui_right * video_size_x() / scale_x;
				ui_left = rs.ui_left * video_size_x() / scale_x;
				ui_top = rs.ui_top * video_size_y() / scale_y;
				ui_bottom = rs.ui_bottom * video_size_y() / scale_y;

				if(rs.mode_get() == mode_custom) {
					
					if (fontsize_X==0)
						fontsize_X =  fontsize_Y;
					fontsize_X = fontsize_X  * video_size_x() / scale_x;
					fontsize_Y = fontsize_Y  * video_size_y() / scale_y;

					if (menu_fontsize_X==0)
						menu_fontsize_X = menu_fontsize_Y;
					menu_fontsize_X = menu_fontsize_X  * video_size_x() / scale_x;
					menu_fontsize_Y = menu_fontsize_Y  * video_size_y() / scale_y;

					ui_list_x = ui_list_x * video_size_x() / scale_x;
					ui_list_y = ui_list_y * video_size_y() / scale_y;
					ui_list_dx = ui_list_dx * video_size_x() / scale_x;
					ui_list_dy = ui_list_dy * video_size_y() / scale_y;
					
					ui_grid_x = ui_grid_x * video_size_x() / scale_x;
					ui_grid_y = ui_grid_y * video_size_y() / scale_y;
					ui_grid_dx = ui_grid_dx * video_size_x() / scale_x;
					ui_grid_dy = ui_grid_dy * video_size_y() / scale_y;
					
					for (int i=0;i<backdrop_win_mac;++i) {
						backdrop_win[i].x = backdrop_win[i].x * video_size_x() / scale_x;
						backdrop_win[i].y = backdrop_win[i].y * video_size_y() / scale_y;
						backdrop_win[i].dx = backdrop_win[i].dx * video_size_x() / scale_x;
						backdrop_win[i].dy = backdrop_win[i].dy * video_size_y() / scale_y;
					}
					
					if(bar_info_1_dx != 0) {
						bar_info_1_x = bar_info_1_x* video_size_x() / scale_x;
						bar_info_1_y = bar_info_1_y * video_size_y() / scale_y;
						bar_info_1_dx = bar_info_1_dx* video_size_x() / scale_x;
						
						bar_info_1_font_dx = bar_info_1_dy * video_size_x()/scale_x;
						bar_info_1_dy = bar_info_1_dy * video_size_y() / scale_y;
					}
					if(bar_info_2_dx != 0) {
						bar_info_2_x = bar_info_2_x* video_size_x() / scale_x;
						bar_info_2_y = bar_info_2_y * video_size_y() / scale_y;
						bar_info_2_dx = bar_info_2_dx* video_size_x() / scale_x;
						
						bar_info_2_font_dx = bar_info_2_dy * video_size_x()/scale_x;
						bar_info_2_dy = bar_info_2_dy * video_size_y() / scale_y;
					}
					if(bar_info_3_dx != 0) {
						bar_info_3_x = bar_info_3_x* video_size_x() / scale_x;
						bar_info_3_y = bar_info_3_y * video_size_y() / scale_y;
						bar_info_3_dx = bar_info_3_dx* video_size_x() / scale_x;
						
						bar_info_3_font_dx = bar_info_3_dy * video_size_x()/scale_x;
						bar_info_3_dy = bar_info_3_dy * video_size_y() / scale_y;
					}
					if(bar_info_4_dx != 0) {
						bar_info_4_x = bar_info_4_x* video_size_x() / scale_x;
						bar_info_4_y = bar_info_4_y * video_size_y() / scale_y;
						bar_info_4_dx = bar_info_4_dx* video_size_x() / scale_x;
						
						bar_info_4_font_dx = bar_info_4_dy * video_size_x()/scale_x;
						bar_info_4_dy = bar_info_4_dy * video_size_y() / scale_y;
					}
					if(bar_info_5_dx != 0) {
						bar_info_5_x = bar_info_5_x* video_size_x() / scale_x;
						bar_info_5_y = bar_info_5_y * video_size_y() / scale_y;
						bar_info_5_dx = bar_info_5_dx * video_size_x() / scale_x;
						
						bar_info_5_font_dx = bar_info_5_dy * video_size_x()/scale_x;
						bar_info_5_dy = bar_info_5_dy * video_size_y() / scale_y;
					}
					
				}
				
			}
			imagen_fondo_cargada = true;
		}
	}

	if (rs.mode_get() == mode_custom) {
		if (backdrop_win_mac > 1) {
			backdrop_win_sort(backdrop_win_mac, backdrop_win);
		}
		for (int i=0;i<backdrop_win_mac;++i) {
			comprobar_medidas(backdrop_win[i].x, backdrop_win[i].y, backdrop_win[i].dx, backdrop_win[i].dy);
		}
		comprobar_medidas(ui_grid_x, ui_grid_y, ui_grid_dx, ui_grid_dy);
		comprobar_medidas(bar_info_1_x, bar_info_1_y, bar_info_1_dx, bar_info_1_dy);
		comprobar_medidas(bar_info_2_x, bar_info_2_y, bar_info_2_dx, bar_info_2_dy);
		comprobar_medidas(bar_info_3_x, bar_info_3_y, bar_info_3_dx, bar_info_3_dy);
		comprobar_medidas(bar_info_4_x, bar_info_4_y, bar_info_4_dx, bar_info_4_dy);
		comprobar_medidas(bar_info_5_x, bar_info_5_y, bar_info_5_dx, bar_info_5_dy);
	}

	int_enable_font_lay(int_font_list, fontsize_X, fontsize_Y, font_path, video_orientation);
	usar_fuente(int_font_list);
	
	if(rs.mode_get() == mode_custom) {
		if(menu_font_path != "none")
			int_enable_font_lay(int_font_menu, menu_fontsize_X, menu_fontsize_Y, menu_font_path, video_orientation);
		if(bar_info_1_dy)
			int_enable_font_info(int_font_info_1, bar_info_1_font_dx, bar_info_1_dy, bar_info_1_font_path, video_orientation);
		if(bar_info_2_dy)
			int_enable_font_info(int_font_info_2, bar_info_2_font_dx, bar_info_2_dy, bar_info_2_font_path, video_orientation);
		if(bar_info_3_dy)
			int_enable_font_info(int_font_info_3, bar_info_3_font_dx, bar_info_3_dy, bar_info_3_font_path, video_orientation);
		if(bar_info_4_dy)
			int_enable_font_info(int_font_info_4, bar_info_4_font_dx, bar_info_4_dy, bar_info_4_font_path, video_orientation);
		if(bar_info_5_dy)
			int_enable_font_info(int_font_info_5, bar_info_5_font_dx, bar_info_5_dy, bar_info_5_font_path, video_orientation);
	}

	scr_x = ui_left;
	scr_y = ui_top;
	scr_dx = video_size_x() - ui_right - scr_x;
	scr_dy = video_size_y() - ui_bottom - scr_y;
	if (scr_dx <= 0 || scr_dy <= 0) {
		scr_x = 0;
		scr_y = 0;
		scr_dx = video_size_x();
		scr_dy = video_size_y();
	}
	int_invrotate(scr_x, scr_y, scr_dx, scr_dy);

	// standard bars
	bar_top_x = scr_x;
	bar_top_y = scr_y;
	if (rs.ui_top_bar)
		bar_top_dy = int_font_dy_get();
	else
		bar_top_dy = 0;
	bar_bottom_x = scr_x;
	if (rs.ui_bottom_bar)
		bar_bottom_dy = int_font_dy_get();
	else
		bar_bottom_dy = 0;
	bar_left_x = scr_x;
	bar_left_y = scr_y + bar_top_dy;
	bar_left_dx = int_font_dx_get()/2;
	bar_right_y = scr_y + bar_top_dy;
	bar_right_dx = int_font_dx_get()/2;

	// effective preview type
	listpreview_t effective_preview;
	switch (rs.mode_get()) {
	case mode_tile_icon : effective_preview = preview_icon; break;
	case mode_tile_marquee : effective_preview = preview_marquee; break;
	default:
		effective_preview = rs.preview_get();
		if (effective_preview == preview_icon || effective_preview == preview_marquee)
			effective_preview = preview_snap;
		// if no image, set another preview
		if ((effective_preview & rs.preview_mask) == 0) {
			if ((preview_snap & rs.preview_mask) != 0)
				effective_preview = preview_snap;
			else if ((preview_title & rs.preview_mask) != 0)
				effective_preview = preview_title;
			else if ((preview_flyer & rs.preview_mask) != 0)
				effective_preview = preview_flyer;
		}
	}

	// cursor
	unsigned cursor_size;
	if (rs.mode_get() != mode_full && rs.mode_get() != mode_list && rs.mode_get() != mode_full_mixed && rs.mode_get() != mode_list_mixed && rs.mode_get() != mode_text && rs.mode_get() != mode_custom) {
		// need a cursor
		cursor_size = video_size_y() / 200 + 1; // size of the flashing cursor
	} else {
		// no cursor
		cursor_size = 0;
	}

	// use identation on the names
	bool use_ident = false;

	if (rs.mode_get() == mode_tile_icon) {
		// icon mode
		unsigned icon_space = rs.icon_space;
		if (icon_space > 64)
			icon_space = 64;
		if (icon_space < 2*cursor_size + int_font_dy_get())
			icon_space = 2*cursor_size + int_font_dy_get();

		space_x = 0;
		space_y = 0;
		name_dy = int_font_dy_get();
		if (!flipxy) {
			coln = (int_dx_get() - bar_left_dx - bar_right_dx) / (32+icon_space);
			rown = (scr_dy - bar_top_dy - bar_bottom_dy) / (32+icon_space);
		} else {
			coln = (int_dx_get() - bar_left_dx - bar_right_dx) / (32+icon_space);
			rown = (scr_dy - bar_top_dy - bar_bottom_dy) / (32+icon_space);
		}

		backdrop_mac = coln*rown;

		win_x = scr_x + bar_left_dx;
		win_y = scr_y + bar_top_dy;

		win_dx = (scr_dx - bar_left_dx - bar_right_dx - (coln-1) * space_x) / coln * coln + (coln-1) * space_x;
		win_dy = (scr_dy - bar_top_dy - bar_bottom_dy - (rown-1) * space_y) / rown * rown + (rown-1) * space_y;

		backdrop_x = 0;
		backdrop_y = 0;
		backdrop_dx = 0;
		backdrop_dy = 0;
		box = false;

		bar_right_x = scr_x + bar_left_dx + win_dx;
		bar_bottom_y = scr_y + bar_top_dy + win_dy;
		bar_top_dx = bar_bottom_dx = bar_left_dx + bar_right_dx + win_dx;
		bar_left_dy = bar_right_dy = win_dy;
	} else if (rs.mode_get() == mode_tile_marquee) {
		// marquee mode
		space_x = int_font_dx_get()/2;
		space_y = int_font_dx_get()/2;
		name_dy = 0;

		if (!flipxy) {
			coln = 3;
			rown = 3*4* 3/4;
		} else {
			coln = 2;
			rown = 2*4* 4/3;
		}

		backdrop_mac = coln*rown;

		win_x = scr_x + bar_left_dx;
		win_y = scr_y + bar_top_dy;

		win_dx = (scr_dx - bar_left_dx - bar_right_dx - (coln-1) * space_x) / coln * coln + (coln-1) * space_x;
		win_dy = (scr_dy - bar_top_dy - bar_bottom_dy - (rown-1) * space_y) / rown * rown + (rown-1) * space_y;

		backdrop_x = 0;
		backdrop_y = 0;
		backdrop_dx = 0;
		backdrop_dy = 0;
		box = false;

		bar_right_x = scr_x + bar_left_dx + win_dx;
		bar_bottom_y = scr_y + bar_top_dy + win_dy;
		bar_top_dx = bar_bottom_dx = bar_left_dx + bar_right_dx + win_dx;
		bar_left_dy = bar_right_dy = win_dy;
	} else if (rs.mode_get() == mode_full || rs.mode_get() == mode_full_mixed) {
		// full mode
		if (rs.mode_get() == mode_full_mixed)
			backdrop_mac = 4;
		else
			backdrop_mac = 1;
		name_dy = 0;

		win_x = scr_x + bar_left_dx;
		win_y = scr_y + bar_top_dy;

		space_x = 0;
		space_y = 0;

		coln = 1;
		rown = 1;
		win_dx = 0;
		win_dy = 0;

		backdrop_x = win_x;
		backdrop_y = win_y;
		backdrop_dx = scr_dx - bar_left_dx - bar_right_dx;
		backdrop_dy = scr_dy - bar_top_dy - bar_bottom_dy;
		box = false;

		bar_right_x = scr_x + bar_left_dx + backdrop_dx;
		bar_bottom_y = scr_y + bar_top_dy + backdrop_dy;
		bar_top_dx = bar_bottom_dx = bar_left_dx + bar_right_dx + backdrop_dx;
		bar_left_dy = bar_right_dy = backdrop_dy;
	} else if (rs.mode_get() == mode_text) {
		// text mode
		backdrop_mac = 0;

		name_dy = int_font_dy_get();

		space_x = int_font_dx_get() / 2;
		space_y = 0;

		coln = scr_dx / (25*int_font_dx_get());
		if (coln < 2)
			coln = 2;
		rown = (scr_dy - bar_top_dy - bar_bottom_dy) / int_font_dy_get();

		win_x = scr_x + bar_left_dx;
		win_y = scr_y + bar_top_dy;
		win_dx = (scr_dx - bar_left_dx - bar_right_dx - (coln-1) * space_x) / coln * coln + (coln-1) * space_x;
		win_dy = rown * int_font_dy_get();

		backdrop_x = 0;
		backdrop_y = 0;
		backdrop_dx = 0;
		backdrop_dy = 0;
		box = false;

		bar_right_x = scr_x + bar_left_dx + win_dx;
		bar_bottom_y = scr_y + bar_top_dy + win_dy;
		bar_top_dx = bar_bottom_dx = bar_left_dx + bar_right_dx + win_dx;
		bar_left_dy = bar_right_dy = win_dy;
	} else if (rs.mode_get() == mode_list || rs.mode_get() == mode_list_mixed) {
		// list mode
		if (rs.mode_get() == mode_list_mixed)
			backdrop_mac = 4;
		else
			backdrop_mac = 1;
		name_dy = int_font_dy_get();

		win_x = scr_x + bar_left_dx;
		win_y = scr_y + bar_top_dy;

		unsigned multiplier;
		unsigned divisor;
		if (rs.mode_get() == mode_list) {
			multiplier = 1;
			divisor = 3;
		} else {
			multiplier = 1;
			divisor = 4;
		}

		if (!flipxy) {
			// horizontal
			use_ident = true;

			space_x = 0;
			space_y = 0;

			coln = 1;
			win_dx = (scr_dx - bar_left_dx - bar_right_dx) * multiplier / divisor;
			win_dx -= win_dx % int_font_dx_get();

			rown = (scr_dy - bar_top_dy - bar_bottom_dy) / int_font_dy_get();
			win_dy = rown * int_font_dy_get();

			backdrop_x = win_x + win_dx;
			backdrop_y = win_y;
			backdrop_dx = scr_dx - win_dx - bar_left_dx - bar_right_dx;
			backdrop_dy = win_dy;
			box = false;

			bar_right_x = scr_x + bar_left_dx + win_dx + backdrop_dx;
			bar_bottom_y = scr_y + bar_top_dy + win_dy;
			bar_top_dx = bar_bottom_dx = bar_left_dx + bar_right_dx + win_dx + backdrop_dx;
			bar_left_dy = bar_right_dy = win_dy;
		} else {
			// vertical
			space_x = int_font_dx_get() / 2;
			space_y = 0;
			coln = scr_dx/(20*int_font_dx_get());
			if (coln < 2)
				coln = 2;
			win_dx = (scr_dx - bar_left_dx - bar_right_dx - (coln-1) * space_x) / coln * coln + (coln-1) * space_x;

			rown = (scr_dy - bar_top_dy - bar_bottom_dy) * multiplier / (divisor * int_font_dy_get());
			win_dy = rown * int_font_dy_get();

			backdrop_x = win_x;
			backdrop_y = win_y + win_dy;
			backdrop_dx = win_dx;
			backdrop_dy = scr_dy - win_dy - bar_top_dy - bar_bottom_dy;
			box = false;

			bar_right_x = scr_x + bar_left_dx + win_dx;
			bar_bottom_y = scr_y + bar_top_dy + win_dy + backdrop_dy;
			bar_top_dx = bar_bottom_dx = bar_left_dx + bar_right_dx + win_dx;
			bar_left_dy = bar_right_dy = win_dy + backdrop_dy;
		}
	} else if (rs.mode_get() == mode_custom) {
		// custom mode
		backdrop_mac = 1;
		name_dy = int_font_dy_get();

		use_ident = true;

		scr_x = 0;
		scr_y = 0;
		scr_dx = video_size_x();
		scr_dy = video_size_y();

		win_x = ui_list_x;
		win_y = ui_list_y;
		win_dx = ui_list_dx;
		win_dy = ui_list_dy;

		coln = cols;
		space_x = space_cols;

		if(rows == "auto") {
			if(space_rows == "auto") {
				space_y = 0;
				rown = win_dy / name_dy;
			} else {
				space_y = atoi(space_rows.c_str());
				rown = (win_dy - space_y) / (name_dy + space_y);
			}
		} else {
			if (space_rows == "auto") {
				rown = atoi(rows.c_str());
				if (rown <= 1)
					space_y = 0;
				else
					space_y = (win_dy - (rown * name_dy)) / (rown - 1);
			} else {
				rown = atoi(rows.c_str());
				space_y = atoi(space_rows.c_str());
			}
		}

		if (frenado) {
			if (row_rel >= rown || col_rel >= coln)
				rel = 0;
			else
				rel = row_rel * coln + col_rel;
		}
		
		bar_left_x = 0;
		bar_left_y = 0;
		bar_left_dx = 0;
		bar_left_dy = ui_grid_dy;

		bar_right_x = ui_grid_x;
		bar_right_y = ui_grid_y;
		bar_right_dx = ui_grid_dx;
		bar_right_dy = ui_grid_dy;

		bar_bottom_y = 0;
		bar_bottom_dx = 465;
		bar_bottom_dy = 0;

		bar_top_dx = 465;
		bar_top_dy = 0;

		box = false;

	} else {
		// tile modes
		space_x = int_font_dx_get()/2;
		space_y = int_font_dx_get()/2;

		if (!flipxy) {
			switch (rs.mode_get()) {
			default: /* for warnings */
			case mode_tile_giant :
				coln = 16;
				rown = 12;
				name_dy = 0;
				space_x = 0;
				space_y = 0;
				bar_left_dx = 0;
				bar_right_dx = 0;
				bar_top_dy = 0;
				bar_bottom_dy = 0;
				break;
			case mode_tile_enormous :
				coln = 12;
				rown = 9;
				name_dy = 0;
				space_x = 0;
				space_y = 0;
				bar_left_dx = 0;
				bar_right_dx = 0;
				bar_top_dy = 0;
				bar_bottom_dy = 0;
				break;
			case mode_tile_big :
				coln = 8;
				rown = 6;
				name_dy = 0;
				space_x = 0;
				space_y = 0;
				break;
			case mode_tile_normal :
				coln = 5;
				rown = 4;
				name_dy = int_font_dy_get();
				break;
			case mode_tile_small :
				coln = 4;
				rown = 3;
				name_dy = int_font_dy_get();
				break;
			}
		} else {
			switch (rs.mode_get()) {
			default: /* for warnings */
			case mode_tile_giant :
				coln = 12;
				rown = 16;
				name_dy = 0;
				space_x = 0;
				space_y = 0;
				bar_left_dx = 0;
				bar_right_dx = 0;
				bar_top_dy = 0;
				bar_bottom_dy = 0;
				break;
			case mode_tile_enormous :
				coln = 9;
				rown = 12;
				name_dy = 0;
				space_x = 0;
				space_y = 0;
				bar_left_dx = 0;
				bar_right_dx = 0;
				bar_top_dy = 0;
				bar_bottom_dy = 0;
				break;
			case mode_tile_big :
				coln = 6;
				rown = 8;
				name_dy = 0;
				space_x = 0;
				space_y = 0;
				break;
			case mode_tile_normal :
				coln = 4;
				rown = 5;
				name_dy = int_font_dy_get();
				break;
			case mode_tile_small :
				coln = 3;
				rown = 4;
				name_dy = int_font_dy_get();
				break;
			}
		}

		backdrop_mac = coln*rown;

		box = name_dy == 0;
		if (box) {
			box_x = 1;
			box_y = 1;
			box_dx = 2;
			box_dy = 2;
		} else {
			box_x = 0;
			box_y = 0;
			box_dx = 0;
			box_dy = 0;
		}

		win_x = scr_x + bar_left_dx + box_x;
		win_y = scr_y + bar_top_dy + box_y;
		win_dx = (scr_dx - bar_left_dx - bar_right_dx - box_dx - (coln-1) * space_x) / coln * coln + (coln-1) * space_x;
		win_dy = (scr_dy - bar_top_dy - bar_bottom_dy - box_dy - (rown-1) * space_y) / rown * rown + (rown-1) * space_y;

		backdrop_x = 0;
		backdrop_y = 0;
		backdrop_dx = 0;
		backdrop_dy = 0;

		bar_right_x = scr_x + bar_left_dx + win_dx + box_dx;
		bar_bottom_y = scr_y + bar_top_dy + win_dy + box_dy;
		bar_top_dx = bar_bottom_dx = bar_left_dx + bar_right_dx + win_dx + box_dx;
		bar_left_dy = bar_right_dy = win_dy + box_dy;

		if (box) {
			box_x = win_x - 1;
			box_y = win_y - 1;
			box_dx = win_dx + 2;
			box_dy = win_dy + 2;
		}
	}

	backdrop_map = new cell_t[backdrop_mac];
	backdrop_map_bis = new cell_t[backdrop_mac];

	// text position
	struct cell_t* int_map = new cell_t[coln*rown];

	if (rs.mode_get() == mode_custom) {
		calculo_listado_5(int_map, coln, rown, win_x, win_y, win_dx, win_dy, name_dy, space_x, space_y, diagonal);
	} else {
		int cell_dx = (win_dx - space_x * (coln-1)) / coln;
		int cell_dy = ((win_dy - space_y * (rown-1)) / rown);

		if (rs.mode_get() == mode_full_mixed || rs.mode_get() == mode_list_mixed) {
			int bar_dx; // size of the additional small images
			int bar_dy;
			int aspect_x;
			int aspect_y;

			if (rs.mode_get() == mode_list_mixed) {
				if (!flipxy) {
					aspect_x = 4 * 4 * video_size_y();
					aspect_y = 3 * 3 * video_size_x();
				} else {
					aspect_x = video_size_y();
					aspect_y = video_size_x();
				}
			} else {
				aspect_x = 4 * video_size_y();
				aspect_y = 3 * video_size_x();
			}

			box = true;
			box_x = backdrop_x;
			box_y = backdrop_y;
			box_dx = backdrop_dx;
			box_dy = backdrop_dy;
			backdrop_x += 1;
			backdrop_y += 1;
			backdrop_dx -= 2;
			backdrop_dy -= 2;

			// game vertical
			bar_dx = backdrop_dx - backdrop_dy * aspect_y / aspect_x;
			if (bar_dx < backdrop_dx / 5)
				bar_dx = backdrop_dx / 5;
			bar_dy = backdrop_dy / 3;
			backdrop_map[0].x = backdrop_x;
			backdrop_map[0].y = backdrop_y;
			backdrop_map[0].dx = backdrop_dx-bar_dx;
			backdrop_map[0].dy = backdrop_dy;
			backdrop_map[1].x = backdrop_x+backdrop_dx-bar_dx;
			backdrop_map[1].y = backdrop_y;
			backdrop_map[1].dx = bar_dx;
			backdrop_map[1].dy = bar_dy;
			backdrop_map[2].x = backdrop_x+backdrop_dx-bar_dx;
			backdrop_map[2].y = backdrop_y+bar_dy;
			backdrop_map[2].dx = bar_dx;
			backdrop_map[2].dy = bar_dy;
			backdrop_map[3].x = backdrop_x+backdrop_dx-bar_dx;
			backdrop_map[3].y = backdrop_y+2*bar_dy;
			backdrop_map[3].dx = bar_dx;
			backdrop_map[3].dy = backdrop_dy-2*bar_dy;

			// game horizontal
			bar_dx = backdrop_dx / 3;
			bar_dy = backdrop_dy - backdrop_dx * aspect_y / aspect_x;
			if (bar_dy < backdrop_dy / 5)
				bar_dy = backdrop_dy / 5;
			backdrop_map_bis[0].x = backdrop_x;
			backdrop_map_bis[0].y = backdrop_y+bar_dy;
			backdrop_map_bis[0].dx = backdrop_dx;
			backdrop_map_bis[0].dy = backdrop_dy-bar_dy;
			backdrop_map_bis[1].x = backdrop_x;
			backdrop_map_bis[1].y = backdrop_y;
			backdrop_map_bis[1].dx = bar_dx;
			backdrop_map_bis[1].dy = bar_dy;
			backdrop_map_bis[2].x = backdrop_x+bar_dx;
			backdrop_map_bis[2].y = backdrop_y;
			backdrop_map_bis[2].dx = bar_dx;
			backdrop_map_bis[2].dy = bar_dy;
			backdrop_map_bis[3].x = backdrop_x+2*bar_dx;
			backdrop_map_bis[3].y = backdrop_y;
			backdrop_map_bis[3].dx = backdrop_dx-2*bar_dx;
			backdrop_map_bis[3].dy = bar_dy;
		}

		if (rs.mode_get() == mode_full_mixed || rs.mode_get() == mode_full) {
			int_map[0].x = 0;
			int_map[0].y = 0;
			int_map[0].dx = 0;
			int_map[0].dy = 0;
		} else if (rs.mode_get() == mode_list || rs.mode_get() == mode_list_mixed || rs.mode_get() == mode_text) {
			for(int r=0;r<rown;++r) {
				for(int c=0;c<coln;++c) {
					unsigned i = r*coln+c;
					int x = win_x + (cell_dx + space_x) * c;
					int y = win_y + (cell_dy + space_y) * r;
					int_map[i].x = x;
					int_map[i].y = y + cell_dy - name_dy;
					int_map[i].dx = cell_dx;
					int_map[i].dy = name_dy;
				}
			}
		} else {
			for(int r=0;r<rown;++r) {
				for(int c=0;c<coln;++c) {
					unsigned i = r*coln+c;
					int x = win_x + (cell_dx + space_x) * c;
					int y = win_y + (cell_dy + space_y) * r;
					if (rs.mode_get() == mode_tile_icon) {
						backdrop_map[i].dx = 32+2*cursor_size;
						backdrop_map[i].dy = 32+2*cursor_size;
						int name_row = 3;
						int space_up = 0; //arreglo bug: mode tile icon
						do {
							--name_row;
							space_up = (cell_dy - backdrop_map[i].dy - name_dy*name_row);
							space_up = space_up/3;
						} while (space_up < 0);
						backdrop_map[i].x = x + (cell_dx - 32 - 2*cursor_size) / 2;
						backdrop_map[i].y = y + space_up;
						int_map[i].x = x;
						int_map[i].y = y + backdrop_map[i].dy + 2*space_up;
						int_map[i].dx = cell_dx;
						int_map[i].dy = name_row*name_dy;
					} else {
						backdrop_map[i].x = x;
						backdrop_map[i].y = y;
						backdrop_map[i].dx = cell_dx;
						backdrop_map[i].dy = cell_dy - name_dy;
						int_map[i].x = x;
						int_map[i].y = y + cell_dy - name_dy;
						int_map[i].dx = cell_dx;
						int_map[i].dy = name_dy;
					}
				}
			}
		}
	}

	if (rs.mode_get() == mode_custom) {
		int_backdrop_init(LCOLOR_WIN, LCOLOR_WIN, backdrop_win_mac, 0, backdrop_win_border, cursor_size, rs.preview_expand, false);
		for(int i=0;i<backdrop_win_mac;++i)
			int_backdrop_pos(i, backdrop_win[i].x, backdrop_win[i].y, backdrop_win[i].dx, backdrop_win[i].dy);
	} else { 
		if (backdrop_mac == 1) {
			int_backdrop_init(COLOR_MENU_BACKDROP, COLOR_MENU_CURSOR, 1, 0, 1, cursor_size, rs.preview_expand, false);
			int_backdrop_pos(0, backdrop_x, backdrop_y, backdrop_dx, backdrop_dy);
		} else if (backdrop_mac > 1) {
			if (rs.mode_get() == mode_tile_icon)
				int_backdrop_init(COLOR_MENU_ICON, COLOR_MENU_CURSOR, backdrop_mac, coln, cursor_size, cursor_size, rs.preview_expand, false);
			else if (rs.mode_get() == mode_list_mixed || rs.mode_get() == mode_full_mixed)
				int_backdrop_init(COLOR_MENU_BACKDROP, COLOR_MENU_CURSOR, backdrop_mac, 1, 0, cursor_size, rs.preview_expand, false);
			else {
				if (space_x == 0)
					int_backdrop_init(COLOR_MENU_BACKDROP, COLOR_MENU_CURSOR, backdrop_mac, coln, 0, cursor_size, rs.preview_expand, rs.clip_mode == clip_multi || rs.clip_mode == clip_multiloop || rs.clip_mode == clip_multiloopall);
				else
					int_backdrop_init(COLOR_MENU_BACKDROP, COLOR_MENU_CURSOR, backdrop_mac, coln, 1, cursor_size, rs.preview_expand, rs.clip_mode == clip_multi || rs.clip_mode == clip_multiloop || rs.clip_mode == clip_multiloopall);
			}
			for(int i=0;i<backdrop_mac;++i)
				int_backdrop_pos(i, backdrop_map[i].x, backdrop_map[i].y, backdrop_map[i].dx, backdrop_map[i].dy);
		}
	}
	
	// reset the sound
	rs.current_sound = resource();

	int pos_rel_max = coln*rown;
	int pos_base_upper = gc.size();
	if (pos_base_upper % coln)
		pos_base_upper = pos_base_upper + coln - pos_base_upper % coln;
	pos_base_upper -= pos_rel_max;
	if (pos_base_upper < 0)
		pos_base_upper = 0;

	// restore the old position
	int pos_base;
	int pos_rel;

	if (rs.current_game) {
		// if a game is selected search the same game
		int i;
		i = 0;

		log_std(("menu: search for game %s\n", rs.current_game->name_get().c_str()));

		while (i < gc.size() && (!gc[i]->has_game() || &gc[i]->game_get() != rs.current_game))
			++i;

		if (i < gc.size()) {
			pos_base = rs.rem_selected ? rs.menu_base_get() : rs.menu_base_effective;
			pos_rel = i - pos_base;
		} else {
			pos_base = rs.rem_selected ? rs.menu_base_get() : rs.menu_base_effective;
			pos_rel = rs.rem_selected ? rs.menu_rel_get() : rs.menu_rel_effective;
		}
	} else {
		pos_base = rs.rem_selected ? rs.menu_base_get() : rs.menu_base_effective;
		pos_rel = rs.rem_selected ? rs.menu_rel_get() : rs.menu_rel_effective;
	}

	if (pos_base < 0) {
		pos_base = pos_base + pos_rel;
		pos_rel = 0;
	}

	// ensure that the position is valid
	if (pos_base + pos_rel >= gc.size()) {
		pos_base = pos_base_upper;
		pos_rel = pos_rel_max - 1;
		if (pos_base + pos_rel >= gc.size()) {
			pos_base = pos_base_upper;
			if (gc.size() > pos_base_upper)
				pos_rel = gc.size() - pos_base_upper - 1;
			else
				pos_rel = 0;
		}
	} else {
		if (pos_base % coln) {
			unsigned rest = pos_base % coln;
			pos_base -= rest;
			pos_rel += rest;
		}
		while (pos_rel < 0) {
			pos_base -= coln;
			pos_rel += coln;
		}
		while (pos_rel >= pos_rel_max) {
			pos_base += coln;
			pos_rel -= coln;
		}
		while (pos_base > pos_base_upper && pos_rel + coln < pos_rel_max) {
			pos_base -= coln;
			pos_rel += coln;
		}
		if (pos_base > pos_base_upper)
			pos_base = pos_base_upper;
	}

	if (frenado) {
		pos_base = pos_base + pos_rel - rel;
		pos_rel = rel;
	}
	
	if (pos_base + pos_rel < gc.size() && gc[pos_base+pos_rel]->has_game()) {
		rs.current_game = &gc[pos_base+pos_rel]->game_get();
		rs.current_clone = 0;
	} else {
		rs.current_game = 0;
		rs.current_clone = 0;
	}

	// count the real games
	{
		int i;
		game_count = 0;
		for(i=0;i<gc.size();++i)
			if (gc[i]->has_game())
				++game_count;
	}

	bool done = false;
	int key = 0;

	// clear the used part
	if (rs.mode_get() == mode_custom) {
		if(!imagen_fondo_cargada) 
			int_clear_alpha(scr_x, scr_y, scr_dx, scr_dy, LCOLOR_BACKGROUND.foreground);
	} else {
		int_clear_alpha(scr_x, scr_y, bar_left_dx + win_dx + bar_right_dx, bar_top_dy + win_dy + bar_bottom_dy, COLOR_MENU_GRID.background);
	}
	
	log_std(("menu: user end\n"));

	while (!done) {
		const game* effective_game = pos_base + pos_rel < gc.size() && gc[pos_base + pos_rel]->has_game() ? &gc[pos_base + pos_rel]->game_get() : 0;

		resource sound;
		if (!sound_find_preview(sound, rs, effective_game)) {
			sound = resource();
		}

#if 0
		/* use the preview as background */
		if (rs.mode_get() == mode_text) {
			resource snap;
			unsigned aspectx, aspecty;

			switch (effective_preview) {
			case preview_cabinet :
			case preview_snap :
			case preview_title :
			case preview_flyer :
				int_clear(COLOR_MENU_GRID.background);
				if (backdrop_find_preview_default(snap, aspectx, aspecty, effective_preview, effective_game, rs)) {
					unsigned scale_x, scale_y;
					int_image(snap.path_get(), scale_x, scale_y);
				}
				int_clear_alpha(scr_x, scr_y, scr_dx, scr_dy, COLOR_MENU_GRID.background);
				break;
			default:
				break;
			}
		}
#endif

		if (name_dy){
			if (rs.mode_get() == mode_custom)
				draw_list_window(rs.gar, gc, int_map, coln, rown, pos_base, pos_base+pos_rel, use_ident, rs.merge, list_align);
			else
				draw_menu_window(rs.gar, gc, int_map, coln, rown, pos_base, pos_base+pos_rel, use_ident, rs.merge, rs.mode_get() == mode_tile_icon);
		}

		game_selected = pos_base + pos_rel + 1;
		
		if (rs.mode_get() == mode_custom) {
			if (bar_info_1_dy)
				draw_bar_info(int_font_info_1, rs.current_game, bar_info_1_text, game_selected, game_count, rs.include_favorites_get(), bar_info_1_x, bar_info_1_y, bar_info_1_dx, bar_info_1_dy, LCOLOR_BAR_INFO_A, bar_info_1_align);
			if (bar_info_2_dy)
				draw_bar_info(int_font_info_2, rs.current_game, bar_info_2_text, game_selected, game_count, rs.include_favorites_get(), bar_info_2_x, bar_info_2_y, bar_info_2_dx, bar_info_2_dy, LCOLOR_BAR_INFO_B, bar_info_2_align);
			if (bar_info_3_dy)
				draw_bar_info(int_font_info_3, rs.current_game, bar_info_3_text, game_selected, game_count, rs.include_favorites_get(), bar_info_3_x, bar_info_3_y, bar_info_3_dx, bar_info_3_dy, LCOLOR_BAR_INFO_C, bar_info_3_align);
			if (bar_info_4_dy)
				draw_bar_info(int_font_info_4, rs.current_game, bar_info_4_text, game_selected, game_count, rs.include_favorites_get(), bar_info_4_x, bar_info_4_y, bar_info_4_dx, bar_info_4_dy, LCOLOR_BAR_INFO_D, bar_info_4_align);
			if (bar_info_5_dy)
				draw_bar_info(int_font_info_5, rs.current_game, bar_info_5_text, game_selected, game_count, rs.include_favorites_get(), bar_info_5_x, bar_info_5_y, bar_info_5_dx, bar_info_5_dy, LCOLOR_BAR_INFO_E, bar_info_5_align);
		}
		
		if (bar_top_dy)
			draw_menu_bar(rs.current_game, rs.include_favorites_get(), game_count, bar_top_x, bar_top_y, bar_top_dx);
		if (bar_bottom_dy)
			draw_menu_info(rs.gar, rs.current_game, bar_bottom_x, bar_bottom_y, bar_bottom_dx, rs.merge, effective_preview, rs.sort_get(), rs.difficulty_effective, rs.lock_effective);
		if (bar_right_dx) {
			if (frenado)
				draw_menu_right(bar_right_x, bar_right_y, bar_right_dx, bar_right_dy, pos_base + rel, pos_rel_max, pos_base_upper + 2 * (pos_rel_max -1));
			else
				draw_menu_right(bar_right_x, bar_right_y, bar_right_dx, bar_right_dy, pos_base, pos_rel_max, pos_base_upper + pos_rel_max);
		}
		if (bar_left_dx)
			int_clear_alpha(bar_left_x, bar_left_y, bar_left_dx, bar_left_dy, COLOR_MENU_BAR.background);
				
		if (rs.mode_get() == mode_full_mixed || rs.mode_get() == mode_list_mixed) {
			bool game_horz = true;

			if (rs.mode_get() == mode_list_mixed && (effective_preview == preview_snap || effective_preview == preview_title)) {
				if (effective_game) {
					if (effective_game->aspectx_get() && effective_game->aspecty_get()) {
						game_horz = effective_game->aspectx_get() > effective_game->aspecty_get();
					} else if (effective_game->sizex_get() && effective_game->sizey_get()) {
						game_horz = effective_game->sizex_get() > effective_game->sizey_get();
					} else {
						game_horz = true;
					}
				}
			} else {
				game_horz = backdrop_dx < backdrop_dy;
			}

			if (!game_horz) {
				for(int i=0;i<backdrop_mac;++i)
					int_backdrop_pos(i, backdrop_map[i].x, backdrop_map[i].y, backdrop_map[i].dx, backdrop_map[i].dy);
			} else {
				for(int i=0;i<backdrop_mac;++i)
					int_backdrop_pos(i, backdrop_map_bis[i].x, backdrop_map_bis[i].y, backdrop_map_bis[i].dx, backdrop_map_bis[i].dy);
			}

			if (effective_preview == preview_title) {
				backdrop_game_set(effective_game, 0, preview_title, true, false, false, rs);
				backdrop_game_set(effective_game, 1, preview_snap, false, false, true, rs);
				backdrop_game_set(effective_game, 2, preview_flyer, false, false, false, rs);
				backdrop_game_set(effective_game, 3, preview_cabinet, false, false, false, rs);
			} else if (effective_preview == preview_flyer) {
				backdrop_game_set(effective_game, 0, preview_flyer, true, false, false, rs);
				backdrop_game_set(effective_game, 1, preview_snap, false, false, true, rs);
				backdrop_game_set(effective_game, 2, preview_title, false, false, false, rs);
				backdrop_game_set(effective_game, 3, preview_cabinet, false, false, false, rs);
			} else if (effective_preview == preview_cabinet) {
				backdrop_game_set(effective_game, 0, preview_cabinet, true, false, false, rs);
				backdrop_game_set(effective_game, 1, preview_snap, false, false, true, rs);
				backdrop_game_set(effective_game, 2, preview_title, false, false, false, rs);
				backdrop_game_set(effective_game, 3, preview_flyer, false, false, false, rs);
			} else {
				backdrop_game_set(effective_game, 0, preview_snap, true, false, true, rs);
				backdrop_game_set(effective_game, 1, preview_title, false, false, false, rs);
				backdrop_game_set(effective_game, 2, preview_flyer, false, false, false, rs);
				backdrop_game_set(effective_game, 3, preview_cabinet, false, false, false, rs);
			}
		} else if (rs.mode_get() == mode_custom) {
			for (int i=0; i<backdrop_win_mac; ++i) 
				if (backdrop_win[i].dy != 0)
					backdrop_game_set(effective_game, i, backdrop_win[i].preview, true, false, true, rs);
		} else {
			if (backdrop_mac == 1) {
				backdrop_game_set(effective_game, 0, effective_preview, true, false, true, rs);
			} else if (backdrop_mac > 1) {
				if (rs.clip_mode == clip_multi || rs.clip_mode == clip_multiloop || rs.clip_mode == clip_multiloopall) {
					// put all the clip in the internal cache
					for(int i=0;i<coln*rown;++i) {
						int_clip_clear(i);
					}
				}
				for(int i=0;i<coln*rown;++i) {
					bool current = i == pos_rel;
					if (rs.clip_mode == clip_multi || rs.clip_mode == clip_multiloop || rs.clip_mode == clip_multiloopall)
						backdrop_index_set(pos_base+i, gc, i, effective_preview, current, current, true, rs);
					else
						backdrop_index_set(pos_base+i, gc, i, effective_preview, current, current, current, rs);
				}
			}
		}
		if (box)
			int_box(box_x, box_y, box_dx, box_dy, 1, COLOR_MENU_BACKDROP.foreground);

		if (over_msg.length()) {
			if(menu_font_path != "none") {
				usar_fuente(int_font_menu);
			}
			unsigned dx, dy;
			int x = int_dx_get() / 2;
			int y = int_dy_get() / 2;
			int border = int_font_dx_get()/2;

			// force an update to draw the first time the backdrop images
			int_update(false);

			dx = int_font_dx_get(over_msg);
			dy = int_font_dy_get();

			int_box(x-2*border-dx/2, y-border, dx+4*border, dy+border*2, 1, COLOR_CHOICE_NORMAL.foreground);
			int_clear(x-2*border-dx/2+1, y-border+1, dx+4*border-2, dy+border*2-2, COLOR_CHOICE_NORMAL.background);
			int_put(x-dx/2, y, dx, over_msg, COLOR_CHOICE_TITLE);

			if(menu_font_path != "none") usar_fuente(int_font_list);
		}

		int_update(rs.mode_get() != mode_full_mixed && rs.mode_get() != mode_list_mixed);

		log_std(("menu: wait begin\n"));

		int_idle_0_enable(rs.current_game && rs.current_game->emulator_get()->is_runnable());
		int_idle_1_enable(true);

		run_background_wait(rs, sound, silent, pos_rel, backdrop_mac);

		// replay the sound and clip
		silent = false;

		log_std(("menu: wait end\n"));

		key = int_event_get(false);

		log_std(("menu: key %d\n", key));

		string oldfast = rs.fast;
		rs.fast.erase();

		if (!frenado)
			key = menu_key(key, pos_base, pos_rel, pos_rel_max, pos_base_upper, coln, gc.size());
		else
			key = menu_key_custom(key, pos_base, pos_rel, pos_rel_max, pos_base_upper, coln, gc.size());
		
		is_loaded = true;
		
		switch (key) {
		case EVENT_INS :
			if (pos_base + pos_rel < gc.size() && pos_base + pos_rel > 0) {
				unsigned new_pos = pos_base + pos_rel - 1;
				string i = gc[new_pos]->category(category_extract);
				while (new_pos>0 && gc[new_pos-1]->category(category_extract)== i)
					--new_pos;
				if (!frenado)
					menu_pos(new_pos, pos_base, pos_rel, pos_rel_max, pos_base_upper, coln, gc.size());
				else
					pos_base = new_pos - pos_rel;
			}
			break;
		case EVENT_DEL :
			if (pos_base + pos_rel < gc.size()) {
				unsigned new_pos = pos_base + pos_rel;
				string i = gc[new_pos]->category(category_extract);
				while (new_pos<gc.size()-1 && gc[new_pos]->category(category_extract)== i)
					++new_pos;
				if (!frenado)
					menu_pos(new_pos, pos_base, pos_rel, pos_rel_max, pos_base_upper, coln, gc.size());
				else
					pos_base = new_pos - pos_rel;
			}
			break;
		default:
			if (key>32 && key<128 && isalnum(key)) {
				oldfast.insert(oldfast.length(), 1, (char)key);
				menu_array::const_iterator i;
				for(i=gc.begin();i!=gc.end();++i) {
					if (menu_fast_compare((*i)->desc_get(), oldfast)) {
						break;
					}
				}
				if (i==gc.end()) {
					for(i=gc.begin();i!=gc.end();++i) {
						if ((*i)->has_game()) {
							const game& g = (*i)->game_get().clone_best_get();
							if (menu_fast_compare(g.name_without_emulator_get(), oldfast)) {
								break;
							}
						}
					}
				}
				if (i!=gc.end()) {
					int pos = i - gc.begin();
					if (!frenado)
						menu_pos(pos, pos_base, pos_rel, pos_rel_max, pos_base_upper, coln, gc.size());
					else
						pos_base = pos - pos_rel;
					rs.fast = oldfast;
				}
			}
			break;
		case EVENT_ENTER :
		case EVENT_CLONE :
			if(menu_font_path != "none") {
				usar_fuente(int_font_menu);
			}
		case EVENT_LOCK :
			done = true;
			break;
		case EVENT_IDLE_0 :
		case EVENT_IDLE_1 :
			done = true;
			break;
		}
		if (!rs.lock_effective)
		switch (key) {
		case EVENT_EMU_NEXT :
		case EVENT_EMU_PRE :
			is_loaded = false;
			done = true;
			break;
		case EVENT_ROTATE :
			if (rs.mode_get() == mode_custom) {
				break;
			}
		case EVENT_SETFAVORITES :
			if (!rs.current_game) break;
		case EVENT_HELP :
		case EVENT_FAVORITES_NEXT :
		case EVENT_TYPE :
		case EVENT_ATTRIB :
		case EVENT_SORT :
		case EVENT_SETTYPE :
		case EVENT_COMMAND :
		case EVENT_MENU :
			if(menu_font_path != "none") {
				usar_fuente(int_font_menu);
			}
		case EVENT_MODE :
		
		case EVENT_PREVIEW :
			done = true;
			break;
		case EVENT_ESC :
			if (rs.exit_mode == exit_normal || rs.exit_mode == exit_all || rs.console_mode)
				done = true;
			break;
		case EVENT_OFF :
			if (rs.exit_mode == exit_shutdown || rs.exit_mode == exit_all)
				done = true;
			break;
		}

		if (pos_rel + pos_base < gc.size() && gc[pos_rel + pos_base]->has_game()) {
			rs.current_game = &gc[pos_rel+pos_base]->game_get();
			rs.current_clone = 0;
		} else {
			rs.current_game = 0;
			rs.current_clone = 0;
		}
	}

	if (key == EVENT_IDLE_0) {
		if (gc.size() > 0) {
			unsigned pos = rand() % gc.size();
			while (pos < gc.size() && !gc[pos]->has_game())
				++pos;
			if (pos < gc.size())
				rs.current_game = &gc[pos]->game_get();
		}
	}

	rs.menu_base_set(pos_base);
	rs.menu_rel_set(pos_rel);
	
	rs.menu_base_effective = pos_base;
	rs.menu_rel_effective = pos_rel;

	delete [] int_map;
	delete [] backdrop_map;
	delete [] backdrop_map_bis;

	if (rs.mode_get() == mode_custom) {
		delete [] backdrop_win;
		if (backdrop_win_mac > 0) 
			int_backdrop_done();
	}

	if (backdrop_mac > 0) {
		int_backdrop_done();
	}

	return key;
}

int run_menu_idle(config_state& rs, menu_array& gc)
{
	bool done = false;
	int key = 0;
	unsigned counter = 0;
	listpreview_t preview = preview_snap;

	switch (rs.idle_saver_type) {
	case saver_snap : preview = preview_snap; break;
	case saver_play : preview = preview_snap; break;
	case saver_flyer : preview = preview_flyer; break;
	case saver_cabinet : preview = preview_cabinet; break;
	case saver_title : preview = preview_title; break;
	default:
		assert(0);
		break;
	}

	int_backdrop_init(COLOR_MENU_BACKDROP, COLOR_MENU_CURSOR, 1, 0, 0, 0, rs.preview_expand, false);

	int_backdrop_pos(0, 0, 0, int_dx_get(), int_dy_get());

	// make the set of available snapshot
	vector<unsigned> avail;
	for(unsigned i=0;i<gc.size();++i) {
		resource backdrop;
		if (gc[i]->has_game()) {
			if (backdrop_find_preview_strict(backdrop, preview, &gc[i]->game_get(), rs.idle_saver_type == saver_play)) {
				avail.insert(avail.end(), i);
			}
		}
	}

	// randomize the set
	for(unsigned i=0;i<avail.size();++i) {
		unsigned j = rand() % avail.size();
		unsigned t = avail[i];
		avail[i] = avail[j];
		avail[j] = t;
	}

	int_idle_0_enable(false);
	int_idle_1_enable(true);

	while (!done) {
		unsigned pos;
		bool found;

		if (avail.size() > 0) {
			pos = avail[counter % avail.size()];
			found = true;
		} else {
			pos = 0;
			found = false;
		}

		resource sound;
		if (found) {
			backdrop_index_set(pos, gc, 0, preview, false, false, true, rs);

			if (rs.idle_saver_type == saver_play) {
				if (!sound_find_preview(sound, rs, &gc[pos]->game_get())) {
					sound = resource();
				}
			}

			int_update_pre();

			draw_menu_desc(gc[pos]->game_get().description_get(), counter);

			int_update_post();
		} else {
			backdrop_game_set(0, 0, preview, false, false, true, rs);

			int_update();
		}

		// next game
		++counter;

		run_background_wait(rs, sound, false, 0, 1);

		key = int_event_get(false);

		if (key != EVENT_IDLE_1) {
			done = true;

			// select the game if the user press enter
			if (found && key == EVENT_ENTER) {
				rs.current_game = &gc[pos]->game_get();
			}
		}
	}

	int_backdrop_done();

	return key;
}

int run_menu_idle_off()
{
	bool done = false;
	int key = 0;

	int_clear(COLOR_MENU_GRID.background);

	target_apm_standby();

	while (!done) {
		int_update();

		key = int_event_get(false);

		if (key != EVENT_IDLE_1)
			done = true;
	}

	target_apm_wakeup();

	return key;
}

int run_menu_sort(config_state& rs, const pgame_sort_set& gss, sort_item_func* category_func, bool flipxy, bool silent, string over_msg)
{
	menu_array gc;

	log_std(("menu: insert begin\n"));

	bool list_mode = rs.mode_get() == mode_list || rs.mode_get() == mode_list_mixed  || rs.mode_get() == mode_custom;
	if (!list_mode || rs.sort_get() == sort_by_name || rs.sort_get() == sort_by_time || rs.sort_get() == sort_by_size || rs.sort_get() == sort_by_session || rs.sort_get() == sort_by_timepersession) {
		gc.reserve(gss.size());
		for(pgame_sort_set::const_iterator i = gss.begin();i!=gss.end();++i) {
			gc.insert(gc.end(), new menu_entry(*i, 0));
		}
	} else if (rs.sort_get() == sort_by_emulator) {
		string category = "dummy";
		gc.reserve(gss.size() + 16);
		for(pgame_sort_set::const_iterator i = gss.begin();i!=gss.end();++i) {
			string new_category = category_func(**i);
			if (new_category != category) {
				category = new_category;
				gc.insert(gc.end(), new menu_entry(category));
			}
			unsigned ident = 1;
			if ((*i)->parent_get()) {
				if ((*i)->software_get())
					ident += 2;
				else
					ident += 1;
			}
			gc.insert(gc.end(), new menu_entry(*i, ident * int_font_dx_get()));
		}
	} else if (rs.sort_get() == sort_by_root_name) {
		gc.reserve(gss.size());
		for(pgame_sort_set::const_iterator i = gss.begin();i!=gss.end();++i) {
			unsigned ident = 0;
			if ((*i)->parent_get()) {
				if ((*i)->software_get())
					ident += 2;
				else
					ident += 1;
			}
			gc.insert(gc.end(), new menu_entry(*i, ident * int_font_dx_get()));
		}
	} else {
		string category = "dummy";
		gc.reserve(gss.size() + 256);
		for(pgame_sort_set::const_iterator i = gss.begin();i!=gss.end();++i) {
			string new_category = category_func(**i);
			if (new_category != category) {
				category = new_category;
				gc.insert(gc.end(), new menu_entry(category));
			}
			gc.insert(gc.end(), new menu_entry(*i, int_font_dx_get()));
		}
	}

	if (gc.empty()) {
		gc.insert(gc.end(), new menu_entry("<empty>"));
	}

	log_std(("menu: insert end\n"));

	bool done = false;
	int key = 0;
	bool idle = false;

	while (!done) {
		if (idle) {
			if (rs.idle_saver_type == saver_shutdown) {
				key = EVENT_OFF_FORCE;
				break;
			}
			if (rs.idle_saver_type == saver_exit) {
				key = EVENT_ESC_FORCE;
				break;
			}
			if (rs.restore == restore_idle)
				rs.restore_load();
			if (rs.idle_saver_type == saver_off)
				run_menu_idle_off();
			else {
				key = run_menu_idle(rs, gc);
				if (key == EVENT_ENTER)
					done = true;
			}
			idle = false;
		}

		if (!done) {
			key = run_menu_user(rs, flipxy, gc, category_func, silent, over_msg);

			// replay the sound and clip
			silent = false;

			switch (key) {
			case EVENT_IDLE_1 :
				idle = true;
				break;
			default:
				done = true;
			}
		}
	}

	for(menu_array::iterator i=gc.begin();i!=gc.end();++i)
		delete *i;

	return key;
}

// ------------------------------------------------------------------------
// Run Info

#define RUNINFO_CHOICE_Y int_dy_get()/10
#define RUNINFO_CHOICE_DY 2*int_font_dy_get()

void run_runinfo(config_state& rs)
{
	int x = int_dx_get() / 2;
	int y = RUNINFO_CHOICE_Y;
	int dy = RUNINFO_CHOICE_DY;
	int border = int_font_dx_get()/2;

	const game* g = rs.current_clone ? rs.current_clone : rs.current_game;
	if (!g)
		return;

	if (rs.ui_gamesaver != saver_off) {
		listpreview_t preview = preview_snap;
		switch (rs.ui_gamesaver) {
		case saver_flyer : preview = preview_flyer; break;
		case saver_cabinet : preview = preview_cabinet; break;
		case saver_title : preview = preview_title; break;
		default: preview = preview_snap; break;
		}

		resource backdrop;
		if (backdrop_find_preview_strict(backdrop, preview, g, false)) {
			int_backdrop_init(COLOR_MENU_BACKDROP, COLOR_MENU_CURSOR, 1, 0, 0, 0, rs.preview_expand, false);
			int_backdrop_pos(0, 0, 0, int_dx_get(), int_dy_get());
			backdrop_game_set(g, 0, preview, false, false, false, rs);
			int_update();
			int_backdrop_done();
		}
	}

	if (rs.ui_gamemsg.length() && rs.ui_gamemsg != "none") {
		unsigned w0;
		unsigned w1;
		unsigned dx;
		string desc = g->description_tree_get();

		w0 = int_font_dx_get(rs.ui_gamemsg);
		w1 = int_font_dx_get(desc);
		if (w0 < w1)
			dx = w1;
		else
			dx = w0;

		int_box(x-2*border-dx/2, y-border, dx+4*border, dy+border*2, 1, COLOR_CHOICE_NORMAL.foreground);
		int_clear(x-2*border-dx/2+1, y-border+1, dx+4*border-2, dy+border*2-2, COLOR_CHOICE_NORMAL.background);

		int_put(x-w0/2, y, w0, rs.ui_gamemsg, COLOR_CHOICE_TITLE);
		y += int_font_dy_get();

		int_put(x-w1/2, y, w1, desc, COLOR_CHOICE_NORMAL);
		y += int_font_dy_get();

		int_update();
	}
}

int run_menu(config_state& rs, bool flipxy, bool silent)
{
	pgame_sort_set* psc;
	sort_item_func* category_func;

	log_std(("menu: sort begin\n"));

	// setup the sorted container
	switch (rs.sort_get()) {
	case sort_by_root_name :
		psc = new pgame_sort_set(sort_by_root_name_func);
		category_func = sort_item_root_name;
		break;
	case sort_by_name :
		psc = new pgame_sort_set(sort_by_name_func);
		category_func = sort_item_name;
		break;
	case sort_by_manufacturer :
		psc = new pgame_sort_set(sort_by_manufacturer_func);
		category_func = sort_item_manufacturer;
		break;
	case sort_by_year :
		psc = new pgame_sort_set(sort_by_year_func);
		category_func = sort_item_year;
		break;
	case sort_by_time :
		psc = new pgame_sort_set(sort_by_time_func);
		category_func = sort_item_time;
		break;
	case sort_by_session :
		psc = new pgame_sort_set(sort_by_session_func);
		category_func = sort_item_session;
		break;
	case sort_by_type :
		psc = new pgame_sort_set(sort_by_type_func);
		category_func = sort_item_type;
		break;
	case sort_by_size :
		psc = new pgame_sort_set(sort_by_size_func);
		category_func = sort_item_size;
		break;
	case sort_by_res :
		psc = new pgame_sort_set(sort_by_res_func);
		category_func = sort_item_res;
		break;
	case sort_by_info :
		psc = new pgame_sort_set(sort_by_info_func);
		category_func = sort_item_info;
		break;
	case sort_by_timepersession :
		psc = new pgame_sort_set(sort_by_timepersession_func);
		category_func = sort_item_timepersession;
		break;
	case sort_by_emulator :
		psc = new pgame_sort_set(sort_by_emulator_func);
		category_func = sort_item_emulator;
		break;
	default:
		return EVENT_NONE;
	}

	string emu_msg;

	// setup the emulator state
	for(pemulator_container::iterator i=rs.emu.begin();i!=rs.emu.end();++i) {
		bool state = false;
		if (rs.include_emu_get().size() == 0) {
			state = true;
		} else {
			for(emulator_container::const_iterator j=rs.include_emu_get().begin();j!=rs.include_emu_get().end();++j) {
				if ((*i)->user_name_get() == *j) {
					state = true;
					break;
				}
			}
		}
		if (state) {
			if (emu_msg.length())
				emu_msg += ", ";
			emu_msg += (*i)->user_name_get();
		}
		(*i)->state_set(state);
	}

	// setup the type state
	for(pcategory_container::iterator i=rs.type.begin();i!=rs.type.end();++i) {
		bool state = false;
		if (rs.include_type_get().size() == 0) {
			state = true;
		} else {
			for(category_container::iterator j=rs.include_type_get().begin();j!=rs.include_type_get().end();++j) {
				if ((*i)->name_get() == *j) {
					state = true;
					break;
				}
			}
		}
		(*i)->state_set(state);
	}

	bool has_emu = false;
	bool has_favorites = false;
	bool has_type = false;
	bool has_filter = false;

	// recompute the preview mask
	rs.preview_mask = 0;

	// select and sort
	for(game_set::const_iterator i=rs.gar.begin();i!=rs.gar.end();++i) {
		// emulator
		if (!i->emulator_get()->state_get())
			continue;

		has_emu = true;

		// game lists
		bool state_favorites = false;
		if (rs.include_favorites_get() == "All Games") {
			state_favorites = true;
		} else if (i->gfavorites_get().size()) {
			for(favorites_container::const_iterator j=i->gfavorites_get().begin();j!=(i)->gfavorites_get().end();++j) {
				string fi = rs.include_favorites_get();
				if (*j == fi)
					state_favorites = true;
			}
		}

		if (!state_favorites)
			continue;

		has_favorites = true;

		// type
		if (rs.include_favorites_get() == "All Games" && !i->type_derived_get()->state_get())
			continue;

		has_type = true;

		// filter
		if (rs.include_favorites_get() == "All Games" && !i->emulator_get()->filter(*i))
			continue;

		has_filter = true;

		psc->insert(&*i);

		// update the preview mask
		if (i->preview_snap_get().is_valid() || i->preview_clip_get().is_valid())
			rs.preview_mask |= preview_snap;
		if (i->preview_flyer_get().is_valid())
			rs.preview_mask |= preview_flyer;
		if (i->preview_cabinet_get().is_valid())
			rs.preview_mask |= preview_cabinet;
		if (i->preview_icon_get().is_valid())
			rs.preview_mask |= preview_icon;
		if (i->preview_marquee_get().is_valid())
			rs.preview_mask |= preview_marquee;
		if (i->preview_title_get().is_valid())
			rs.preview_mask |= preview_title;
	}

	/* prepare a warning message if the game list is empty */
	string empty_msg;
	if (rs.gar.empty())
		empty_msg = "No game was found";
	else if (!has_emu)
		empty_msg = "No game was found for the emulator " + emu_msg;
	else if (!has_favorites)
		empty_msg = "No games in list \"" + rs.include_favorites_get() + "\" for emulator \"" + emu_msg + "\"";
	else if (!has_type)
		empty_msg = "No game matches the type selection for " + emu_msg;
	else if (!has_filter)
		empty_msg = "No game matches the filter selection for " + emu_msg;
	else
		empty_msg = "";

	rs.mode_mask = ~rs.mode_skip_mask & (mode_full | mode_full_mixed
		| mode_text | mode_list | mode_list_mixed | mode_tile_small
		| mode_tile_normal | mode_tile_big | mode_tile_enormous
		| mode_tile_giant | mode_tile_icon | mode_tile_marquee | mode_custom);

	// remove some modes if the corresponding preview are not available
	if ((rs.preview_mask & preview_icon) == 0)
		rs.mode_mask &= ~mode_tile_icon;
	if ((rs.preview_mask & preview_marquee) == 0)
		rs.mode_mask &= ~mode_tile_marquee;
	rs.preview_mask &= ~(preview_icon | preview_marquee);

	if (rs.preview_mask == 0)
		rs.mode_mask = mode_text | mode_custom;

	if (rs.mode_mask == 0)
		rs.mode_mask = mode_text;

	log_std(("menu: sort end\n"));

	bool done = false;
	int key = 0;

	while (!done) {
		key = run_menu_sort(rs, *psc, category_func, flipxy, silent, empty_msg);

		// don't replay the sound and clip
		silent = true;

		if (!rs.lock_effective)
		switch (key) {
		case EVENT_MODE :
			if (rs.mode_mask) {
				do {
					switch (rs.mode_get()) {
					case mode_full : rs.mode_set(mode_full_mixed); break;
					case mode_full_mixed : rs.mode_set(mode_text); break;
					case mode_text : rs.mode_set(mode_list); break;
					case mode_list : rs.mode_set(mode_list_mixed); break;
					case mode_list_mixed : rs.mode_set(mode_tile_small); break;
					case mode_tile_small : rs.mode_set(mode_tile_normal); break;
					case mode_tile_normal : rs.mode_set(mode_tile_big); break;
					case mode_tile_big : rs.mode_set(mode_tile_enormous); break;
					case mode_tile_enormous : rs.mode_set(mode_tile_giant); break;
					case mode_tile_giant : rs.mode_set(mode_tile_icon); break;
					case mode_tile_icon : rs.mode_set(mode_tile_marquee); break;
					case mode_tile_marquee : 
						rs.mode_set(mode_custom);	break;
					case mode_custom : rs.mode_set(mode_full); break;													
					}
				} while ((rs.mode_get() & rs.mode_mask) == 0);
			}
			break;
		case EVENT_PREVIEW :
			if (rs.mode_get() != mode_custom && rs.preview_mask) {
				do {
					switch (rs.preview_get()) {
						case preview_icon :
						case preview_marquee :
						case preview_snap : rs.preview_set(preview_title); break;
						case preview_title : rs.preview_set(preview_flyer); break;
						case preview_flyer : rs.preview_set(preview_cabinet); break;
						case preview_cabinet : rs.preview_set(preview_snap); break;
					}
				} while ((rs.preview_get() & rs.preview_mask) == 0);
			}
			break;
		}

		switch (key) {
		case EVENT_ESC :
		case EVENT_OFF :
		case EVENT_ESC_FORCE :
		case EVENT_OFF_FORCE :
			disable_fonts();
		case EVENT_ENTER :
		case EVENT_CLONE :
		case EVENT_IDLE_0 :
		case EVENT_IDLE_1 :
		case EVENT_LOCK :
		case EVENT_HELP :
		case EVENT_FAVORITES_NEXT :
		case EVENT_TYPE :
		case EVENT_ATTRIB :
		case EVENT_SORT :
		case EVENT_SETFAVORITES :
		case EVENT_SETTYPE :
		case EVENT_COMMAND :
		case EVENT_MENU :
		case EVENT_EMU_NEXT :
		case EVENT_EMU_PRE :
		case EVENT_ROTATE :
			done = true;
			break;
		}
	}

	delete psc;

	return key;
}
