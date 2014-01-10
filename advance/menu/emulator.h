/*
 * This file is part of the Advance project.
 *
 * Copyright (C) 1999, 2000, 2001, 2002, 2003 Andrea Mazzoleni
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

#ifndef __EMULATOR_H
#define __EMULATOR_H

#include "game.h"
#include "crc.h"
#include "choice.h"
#include "conf.h"

#include <time.h>

#include <string>

struct config_emulator_state;

class emulator {
	mutable config_emulator_state* config; ///< Pointer at the emulator configuration.
	bool state; ///< If the emulator is listed or not.

	emulator();
protected:
	tristate_t exclude_missing_effective;
	tristate_t exclude_missing_orig;

	std::string attrib_compile(const std::string& value0, const std::string& value1);

	std::string name;

	std::string user_exe_path; // emulator path (in OS format)
	std::string config_exe_path; // absolute emulator path (in UNIVERSAL format)

	std::string user_cmd_arg; // argument to pass at the emulator

	// from the emulator config (in UNIVERSAL format)
	std::string emu_rom_path;
	std::string emu_software_path;
	std::string emu_snap_path;

	// user definable (in OS format)
	std::string user_rom_path;
	std::string user_alts_path;
	std::string user_icon_path;
	std::string user_flyer_path;
	std::string user_cabinet_path;
	std::string user_marquee_path;
	std::string user_title_path;
	std::string user_rom_filter;

	std::string custom_file_path;
	std::string nocustom_background_path;
	std::string nocustom_help_path;
	std::string nocustom_start_path;
	std::string nocustom_font_path;
	std::string nocustom_font_size;
	std::string nocustom_font_color;
	std::string nocustom_font_color_select;
	std::string custom_background_path;
	std::string custom_help_path;
	std::string custom_start_path;
	std::string custom_font_path;
	std::string custom_list_pos_size;
	std::string custom_scroll_pos_size;
	std::string custom_list_cols;
	std::string custom_list_rows;
	std::string custom_list_diagonal;
	std::string custom_list_selected_pos;
	std::string custom_list_align;
	std::string custom_color_font;
	std::string custom_color_font_select;
	std::string custom_color_scroll;
	std::string custom_font_size;
	std::string custom_win_snaps;
	std::string custom_win_flyers;
	std::string custom_win_cabinets;
	std::string custom_win_icons;
	std::string custom_win_marquees;
	std::string custom_win_titles;
	std::string custom_win_color;
	std::string custom_bar_info_1;
	std::string custom_bar_info_1_text;
	std::string custom_bar_info_1_color;
	std::string custom_bar_info_1_font;
	std::string custom_bar_info_1_align;
	std::string custom_bar_info_2;
	std::string custom_bar_info_2_text;
	std::string custom_bar_info_2_color;
	std::string custom_bar_info_2_font;
	std::string custom_bar_info_2_align;
	std::string custom_bar_info_3;
	std::string custom_bar_info_3_text;
	std::string custom_bar_info_3_color;
	std::string custom_bar_info_3_font;
	std::string custom_bar_info_3_align;
	std::string custom_bar_info_4;
	std::string custom_bar_info_4_text;
	std::string custom_bar_info_4_color;
	std::string custom_bar_info_4_font;
	std::string custom_bar_info_4_align;
	std::string custom_bar_info_5;
	std::string custom_bar_info_5_text;
	std::string custom_bar_info_5_color;
	std::string custom_bar_info_5_font;
	std::string custom_bar_info_5_align;
	std::string custom_background_color;
	std::string custom_menu_font_path;
	std::string custom_menu_font_size;
	std::string custom_menu_font_color;
	std::string custom_menu_font_select_color;
	std::string custom_menu_title_color;
	std::string custom_video_orientation;

	// final version = user + emulator config (in UNIVERSAL format)
	std::string config_rom_path;
	std::string config_alts_path;
	std::string config_icon_path;
	std::string config_flyer_path;
	std::string config_cabinet_path;
	std::string config_marquee_path;
	std::string config_title_path;
	
	bool has_atleastarom; // at least a rom was found

	void scan_game(const game_set& gar, const std::string& path, const std::string& name);
	void scan_dir(const game_set& gar, const std::string& dir, bool quiet);
	void scan_dirlist(const game_set& gar, const std::string& dirlist, bool quiet);

	void load_dir(game_set& gar, const std::string& dir, const std::string& filterlist, bool quiet);
	void load_dirlist(game_set& gar, const std::string& dirlist, const std::string& filterlist, bool quiet);

	bool run_process(time_t& duration, const std::string& dir, int argc, const char** argv, bool ignore_error) const;
	unsigned compile(const game& g, const char** argv, unsigned argc, const std::string& list, unsigned orientation) const;

	bool validate_config_file(const std::string& file) const;

public:
	emulator(const std::string& Aname, const std::string& Aexe_path, const std::string& Acmd_arg);
	virtual ~emulator();

	config_emulator_state& config_get() const { return *config; }

	void state_set(bool Astate) { state = Astate; }
	bool state_get() const { return state; }

	static const unsigned flag_derived_vector = game::flag_user;
	static const unsigned flag_derived_vertical = game::flag_user << 1;
	static const unsigned flag_derived_resource = game::flag_user << 2;
	static const unsigned flag_derived_alias = game::flag_user << 3;
	static const unsigned flag_derived_neogeo = game::flag_user << 4;
	static const unsigned flag_derived_deco = game::flag_user << 5;
	static const unsigned flag_derived_playchoice = game::flag_user << 6;
	static const unsigned flag_derived_mechanical = game::flag_user << 7;
	static const unsigned flag_derived_mahjong = game::flag_user << 8;
	static const unsigned flag_derived_beatmania = game::flag_user << 9;
	static const unsigned flag_derived_pokercasino = game::flag_user << 10;
	static const unsigned flag_derived_quiztrivial = game::flag_user << 11;
	static const unsigned flag_derived_golfdarfish = game::flag_user << 12;

	virtual int attrib_run(int x, int y) = 0;
	virtual void attrib_load();
	virtual void attrib_save();
	virtual bool attrib_set(const std::string& value0, const std::string& value1);
	virtual void attrib_get(adv_conf* config_context, const char* section, const char* tag);

	virtual bool filter(const game& g) const;
	virtual void cache(const game_set& gar, const game& g) const;
	virtual bool tree_get() const = 0;

	const std::string& user_name_get() const { return name; }
	const std::string& user_exe_path_get() const { return user_exe_path; }
	const std::string& user_cmd_arg_get() const { return user_cmd_arg; }
	const std::string& config_exe_path_get() const { return config_exe_path; }
	std::string exe_dir_get() const;

	// from the mm.cfg
	void user_rom_path_set(const std::string& A) { user_rom_path = A; }
	const std::string& user_rom_path_get() const { return user_rom_path; }
	void user_alts_path_set(const std::string& A) { user_alts_path = A; }
	const std::string& user_alts_path_get() const { return user_alts_path; }
	void user_icon_path_set(const std::string& A) { user_icon_path = A; }
	const std::string& user_icon_path_get() const { return user_icon_path; }
	void user_flyer_path_set(const std::string& A) { user_flyer_path = A; }
	const std::string& user_flyer_path_get() const { return user_flyer_path; }
	void user_cabinet_path_set(const std::string& A) { user_cabinet_path = A; }
	const std::string& user_cabinet_path_get() const { return user_cabinet_path; }
	void user_marquee_path_set(const std::string& A) { user_marquee_path = A; }
	const std::string& user_marquee_path_get() const { return user_marquee_path; }
	void user_title_path_set(const std::string& A) { user_title_path = A; }
	const std::string& user_title_path_get() const { return user_title_path; }
	void user_rom_filter_set(const std::string& A) { user_rom_filter = A; }
	const std::string& user_rom_filter_get() const { return user_rom_filter; }
	void custom_file_path_set(const std::string& A) { custom_file_path = A; }
	const std::string& custom_file_path_get() const { return custom_file_path; }
	void nocustom_background_path_set(const std::string& A) { nocustom_background_path = A; }
	const std::string& nocustom_background_path_get() const { return nocustom_background_path; }
	void nocustom_help_path_set(const std::string& A) { nocustom_help_path = A; }
	const std::string& nocustom_help_path_get() const { return nocustom_help_path; }
	void nocustom_start_path_set(const std::string& A) { nocustom_start_path = A; }
	const std::string& nocustom_start_path_get() const { return nocustom_start_path; }	
	void nocustom_font_path_set(const std::string& A) { nocustom_font_path = A; }
	const std::string& nocustom_font_path_get() const { return nocustom_font_path; }
	void nocustom_font_size_set(const std::string& A) { nocustom_font_size = A; }
	const std::string& nocustom_font_size_get() const { return nocustom_font_size; }
	void nocustom_font_color_set(const std::string& A) { nocustom_font_color = A; }
	const std::string& nocustom_font_color_get() const { return nocustom_font_color; }
	void nocustom_font_color_select_set(const std::string& A) { nocustom_font_color_select = A; }
	const std::string& nocustom_font_color_select_get() const { return nocustom_font_color_select; }
	void custom_background_path_set(const std::string& A) { custom_background_path = A; }
	const std::string& custom_background_path_get() const { return custom_background_path; }
	void custom_help_path_set(const std::string& A) { custom_help_path = A; }
	const std::string& custom_help_path_get() const { return custom_help_path; }
	void custom_start_path_set(const std::string& A) { custom_start_path = A; }
	const std::string& custom_start_path_get() const { return custom_start_path; }
	void custom_font_path_set(const std::string& A) { custom_font_path = A; }
	const std::string& custom_font_path_get() const { return custom_font_path; }
	void custom_list_pos_size_set(const std::string& A) { custom_list_pos_size = A; }
	const std::string& custom_list_pos_size_get() const { return custom_list_pos_size; }
	void custom_scroll_pos_size_set(const std::string& A) { custom_scroll_pos_size = A; }
	const std::string& custom_scroll_pos_size_get() const { return custom_scroll_pos_size; }
	void custom_list_cols_set(const std::string& A) { custom_list_cols = A; }
	const std::string& custom_list_cols_get() const { return custom_list_cols; }
	void custom_list_rows_set(const std::string& A) { custom_list_rows = A; }
	const std::string& custom_list_rows_get() const { return custom_list_rows; }
	void custom_list_diagonal_set(const std::string& A) { custom_list_diagonal = A; }
	const std::string& custom_list_diagonal_get() const { return custom_list_diagonal; }
	void custom_list_selected_pos_set(const std::string& A) { custom_list_selected_pos = A; }
	const std::string& custom_list_selected_pos_get() const { return custom_list_selected_pos; }
	void custom_list_align_set(const std::string& A) { custom_list_align = A; }
	const std::string& custom_list_align_get() const { return custom_list_align; }
	void custom_color_font_set(const std::string& A) { custom_color_font = A; }
	const std::string& custom_color_font_get() const { return custom_color_font; }
	void custom_color_font_select_set(const std::string& A) { custom_color_font_select = A; }
	const std::string& custom_color_font_select_get() const { return custom_color_font_select; }
	void custom_color_scroll_set(const std::string& A) { custom_color_scroll = A; }
	const std::string& custom_color_scroll_get() const { return custom_color_scroll; }
	void custom_font_size_set(const std::string& A) { custom_font_size = A; }
	const std::string& custom_font_size_get() const { return custom_font_size; }
	void custom_win_snaps_set(const std::string& A) { custom_win_snaps = A; }
	const std::string& custom_win_snaps_get() const { return custom_win_snaps; }
	void custom_win_flyers_set(const std::string& A) { custom_win_flyers = A; }
	const std::string& custom_win_flyers_get() const { return custom_win_flyers; }
	void custom_win_cabinets_set(const std::string& A) { custom_win_cabinets = A; }
	const std::string& custom_win_cabinets_get() const { return custom_win_cabinets; }
	void custom_win_icons_set(const std::string& A) { custom_win_icons = A; }
	const std::string& custom_win_icons_get() const { return custom_win_icons; }
	void custom_win_marquees_set(const std::string& A) { custom_win_marquees = A; }
	const std::string& custom_win_marquees_get() const { return custom_win_marquees; }
	void custom_win_titles_set(const std::string& A) { custom_win_titles = A; }
	const std::string& custom_win_titles_get() const { return custom_win_titles; }
	void custom_win_color_set(const std::string& A) { custom_win_color = A; }
	const std::string& custom_win_color_get() const { return custom_win_color; }
	void custom_bar_info_1_set(const std::string& A) { custom_bar_info_1 = A; }
	const std::string& custom_bar_info_1_get() const { return custom_bar_info_1; }
	void custom_bar_info_1_text_set(const std::string& A) { custom_bar_info_1_text = A; }
	const std::string& custom_bar_info_1_text_get() const { return custom_bar_info_1_text; }
	void custom_bar_info_1_color_set(const std::string& A) { custom_bar_info_1_color = A; }
	const std::string& custom_bar_info_1_color_get() const { return custom_bar_info_1_color; }
	void custom_bar_info_1_font_set(const std::string& A) { custom_bar_info_1_font = A; }
	const std::string& custom_bar_info_1_font_get() const { return custom_bar_info_1_font; }
	void custom_bar_info_1_align_set(const std::string& A) { custom_bar_info_1_align = A; }
	const std::string& custom_bar_info_1_align_get() const { return custom_bar_info_1_align; }
	void custom_bar_info_2_set(const std::string& A) { custom_bar_info_2 = A; }
	const std::string& custom_bar_info_2_get() const { return custom_bar_info_2; }
	void custom_bar_info_2_text_set(const std::string& A) { custom_bar_info_2_text = A; }
	const std::string& custom_bar_info_2_text_get() const { return custom_bar_info_2_text; }
	void custom_bar_info_2_color_set(const std::string& A) { custom_bar_info_2_color = A; }
	const std::string& custom_bar_info_2_color_get() const { return custom_bar_info_2_color; }
	void custom_bar_info_2_font_set(const std::string& A) { custom_bar_info_2_font = A; }
	const std::string& custom_bar_info_2_font_get() const { return custom_bar_info_2_font; }
	void custom_bar_info_2_align_set(const std::string& A) { custom_bar_info_2_align = A; }
	const std::string& custom_bar_info_2_align_get() const { return custom_bar_info_2_align; }
	void custom_bar_info_3_set(const std::string& A) { custom_bar_info_3 = A; }
	const std::string& custom_bar_info_3_get() const { return custom_bar_info_3; }
	void custom_bar_info_3_text_set(const std::string& A) { custom_bar_info_3_text = A; }
	const std::string& custom_bar_info_3_text_get() const { return custom_bar_info_3_text; }
	void custom_bar_info_3_color_set(const std::string& A) { custom_bar_info_3_color = A; }
	const std::string& custom_bar_info_3_color_get() const { return custom_bar_info_3_color; }
	void custom_bar_info_3_font_set(const std::string& A) { custom_bar_info_3_font = A; }
	const std::string& custom_bar_info_3_font_get() const { return custom_bar_info_3_font; }
	void custom_bar_info_3_align_set(const std::string& A) { custom_bar_info_3_align = A; }
	const std::string& custom_bar_info_3_align_get() const { return custom_bar_info_3_align; }
	void custom_bar_info_4_set(const std::string& A) { custom_bar_info_4 = A; }
	const std::string& custom_bar_info_4_get() const { return custom_bar_info_4; }
	void custom_bar_info_4_text_set(const std::string& A) { custom_bar_info_4_text = A; }
	const std::string& custom_bar_info_4_text_get() const { return custom_bar_info_4_text; }
	void custom_bar_info_4_color_set(const std::string& A) { custom_bar_info_4_color = A; }
	const std::string& custom_bar_info_4_color_get() const { return custom_bar_info_4_color; }
	void custom_bar_info_4_font_set(const std::string& A) { custom_bar_info_4_font = A; }
	const std::string& custom_bar_info_4_font_get() const { return custom_bar_info_4_font; }
	void custom_bar_info_4_align_set(const std::string& A) { custom_bar_info_4_align = A; }
	const std::string& custom_bar_info_4_align_get() const { return custom_bar_info_4_align; }
	void custom_bar_info_5_set(const std::string& A) { custom_bar_info_5 = A; }
	const std::string& custom_bar_info_5_get() const { return custom_bar_info_5; }
	void custom_bar_info_5_text_set(const std::string& A) { custom_bar_info_5_text = A; }
	const std::string& custom_bar_info_5_text_get() const { return custom_bar_info_5_text; }
	void custom_bar_info_5_color_set(const std::string& A) { custom_bar_info_5_color = A; }
	const std::string& custom_bar_info_5_color_get() const { return custom_bar_info_5_color; }
	void custom_bar_info_5_font_set(const std::string& A) { custom_bar_info_5_font = A; }
	const std::string& custom_bar_info_5_font_get() const { return custom_bar_info_5_font; }
	void custom_bar_info_5_align_set(const std::string& A) { custom_bar_info_5_align = A; }
	const std::string& custom_bar_info_5_align_get() const { return custom_bar_info_5_align; }
	void custom_background_color_set(const std::string& A) { custom_background_color = A; }
	const std::string& custom_background_color_get() const { return custom_background_color; }
	void custom_menu_font_path_set(const std::string& A) { custom_menu_font_path = A; }
	const std::string& custom_menu_font_path_get() const { return custom_menu_font_path; }
	void custom_menu_font_size_set(const std::string& A) { custom_menu_font_size = A; }
	const std::string& custom_menu_font_size_get() const { return custom_menu_font_size; }
	void custom_menu_title_color_set(const std::string& A) { custom_menu_title_color = A; }
	const std::string& custom_menu_title_color_get() const { return custom_menu_title_color; }
	void custom_menu_font_color_set(const std::string& A) { custom_menu_font_color = A; }
	const std::string& custom_menu_font_color_get() const { return custom_menu_font_color; }
	void custom_menu_font_select_color_set(const std::string& A) { custom_menu_font_select_color = A; }
	const std::string& custom_menu_font_select_color_get() const { return custom_menu_font_select_color; }
	void custom_video_orientation_set(const std::string& A) { custom_video_orientation = A; }
	const std::string& custom_video_orientation_get() const { return custom_video_orientation; }
	// from the emulator.cfg
	const std::string& software_path_get() const { return emu_software_path; }
	const std::string& snap_path_get() const { return emu_snap_path; }

	// final value
	std::string config_rom_path_get() const { return config_rom_path; }
	std::string config_alts_path_get() const { return config_alts_path; }
	std::string config_icon_path_get() const { return config_icon_path; }
	std::string config_flyer_path_get() const { return config_flyer_path; }
	std::string config_cabinet_path_get() const { return config_cabinet_path; }
	std::string config_marquee_path_get() const { return config_marquee_path; }
	std::string config_title_path_get() const { return config_title_path; }
	
	unsigned preview_set(game_set& gar) const;

	virtual bool run(const game& g, const game* bios, unsigned orientation, bool set_difficulty, difficulty_t difficulty, bool set_attenuation, int attenuation, bool ignore_error) const;
	virtual bool load_cfg(const game_set& gar, bool quiet) = 0;
	virtual bool load_data(const game_set& gar) = 0;
	virtual bool load_game(game_set& gar, bool quiet) = 0;
	virtual bool load_software(game_set& gar, bool quiet) = 0;
	virtual void update(const game& g) const;

	virtual bool is_present() const;
	virtual bool is_runnable() const;
	virtual bool is_empty() const;
	virtual std::string type_get() const = 0;
};

class mame_info : public emulator {
protected:
	tristate_t exclude_clone_effective;
	tristate_t exclude_clone_orig;
	tristate_t exclude_bad_effective;
	tristate_t exclude_bad_orig;
	tristate_t exclude_vector_effective;
	tristate_t exclude_vector_orig;
	tristate_t exclude_vertical_effective;
	tristate_t exclude_vertical_orig;

	bool load_info(game_set& gar);
	bool load_xml(std::istream& is, game_set& gar);
	bool load_game_info(game_set& gar);
	bool load_game_xml(game_set& gar);
	bool update_info();
	bool is_update_info();
	bool update_xml();
	bool is_update_xml();
	bool is_present_info();
	bool is_present_xml();
public:
	mame_info(const std::string& Aname, const std::string& Aexe_path, const std::string& Acmd_arg);

	virtual void attrib_load();
	virtual void attrib_save();
	virtual bool attrib_set(const std::string& value0, const std::string& value1);
	virtual void attrib_get(adv_conf* config_context, const char* section, const char* tag);
	virtual bool filter(const game& g) const;
	virtual void cache(const game_set& gar, const game& g) const;
	virtual bool tree_get() const;

	virtual bool load_game(game_set& gar, bool quiet);
	virtual void update(const game& g) const;
};

class mame_mess : public mame_info {
protected:
	tristate_t exclude_empty_effective;
	tristate_t exclude_empty_orig;
public:
	mame_mess(const std::string& Aname, const std::string& Aexe_path, const std::string& Acmd_arg);

	virtual void attrib_load();
	virtual void attrib_save();
	virtual bool attrib_set(const std::string& value0, const std::string& value1);
	virtual void attrib_get(adv_conf* config_context, const char* section, const char* tag);
	virtual bool filter(const game& g) const;

	virtual int attrib_run(int x, int y);
};

class mame_mame : public mame_info {
protected:
	tristate_t exclude_neogeo_effective;
	tristate_t exclude_neogeo_orig;
	tristate_t exclude_deco_effective;
	tristate_t exclude_deco_orig;
	tristate_t exclude_playchoice_effective;
	tristate_t exclude_playchoice_orig;
	tristate_t exclude_mechanical_effective;
	tristate_t exclude_mechanical_orig;
	tristate_t exclude_mahjong_effective;
	tristate_t exclude_mahjong_orig;
	tristate_t exclude_beatmania_effective;
	tristate_t exclude_beatmania_orig;
	tristate_t exclude_pokercasino_effective;
	tristate_t exclude_pokercasino_orig;
	tristate_t exclude_quiztrivial_effective;
	tristate_t exclude_quiztrivial_orig;
	tristate_t exclude_golfdarfish_effective;
	tristate_t exclude_golfdarfish_orig;
	bool support_difficulty;
	bool support_attenuation;
public:
	mame_mame(const std::string& Aname, const std::string& Aexe_path, const std::string& Acmd_arg, bool Asupport_difficulty, bool Asupport_attenuation);

	virtual int attrib_run(int x, int y);
	virtual void attrib_load();
	virtual void attrib_save();
	virtual bool attrib_set(const std::string& value0, const std::string& value1);
	virtual void attrib_get(adv_conf* config_context, const char* section, const char* tag);
	virtual bool filter(const game& g) const;
	virtual void cache(const game_set& gar, const game& g) const;

	virtual bool run(const game& g, const game* bios, unsigned orientation, bool set_difficulty, difficulty_t difficulty, bool set_attenuation, int attenuation, bool ignore_error) const;
	virtual bool load_data(const game_set& gar);
	virtual bool load_software(game_set& gar, bool quiet);
};

class dmame : public mame_mame {
public:
	dmame(const std::string& Aname, const std::string& Aexe_path, const std::string& Acmd_arg);

	virtual bool load_cfg(const game_set& gar, bool quiet);
	virtual std::string type_get() const;
};

class wmame : public mame_mame {
public:
	wmame(const std::string& Aname, const std::string& Aexe_path, const std::string& Acmd_arg);

	virtual bool load_cfg(const game_set& gar, bool quiet);
	virtual std::string type_get() const;
};

class sdlmame : public mame_mame {
public:
	sdlmame(const std::string& Aname, const std::string& Aexe_path, const std::string& Acmd_arg);

	virtual bool load_cfg(const game_set& gar, bool quiet);
	virtual std::string type_get() const;
};

class advmame : public mame_mame {
public:
	advmame(const std::string& Aname, const std::string& Aexe_path, const std::string& Acmd_arg);

	virtual bool load_cfg(const game_set& gar, bool quiet);
	virtual std::string type_get() const;
};

class advpac : public mame_mame {
public:
	advpac(const std::string& Aname, const std::string& Aexe_path, const std::string& Acmd_arg);

	virtual bool load_cfg(const game_set& gar, bool quiet);
	virtual std::string type_get() const;
};

class advmess : public mame_mess {
	static std::string image_name_get(const std::string& snap_create, const std::string& name);
	static std::string clip_name_get(const std::string& clip_create, const std::string& name);
	static std::string sound_name_get(const std::string& sound_create, const std::string& name);

	bool compile_ext(const game& g, unsigned& argc, const char* argv[], const std::string& ext) const;
	bool compile_zip(const game& g, unsigned& argc, const char* argv[], const std::string& zip_file) const;
	bool compile_single(const game& g, unsigned& argc, const char* argv[], const std::string& file) const;
	bool compile_file(const game& g, unsigned& argc, const char* argv[], const std::string& file) const;

	void scan_software_by_sys(game_container& gac, const std::string& software, const game& bios);
	void scan_software(game_container& gac, const game_set& gar);
public:
	advmess(const std::string& Aname, const std::string& Aexe_path, const std::string& Acmd_arg);

	virtual bool run(const game& g, const game* bios, unsigned orientation, bool set_difficulty, difficulty_t difficulty, bool set_attenuation, int attenuation, bool ignore_error) const;
	virtual bool load_cfg(const game_set& gar, bool quiet);
	virtual bool load_data(const game_set& gar);
	virtual bool load_software(game_set& gar, bool quiet);

	virtual std::string type_get() const;
};

class dmess : public mame_mess {
	static std::string image_name_get(const std::string& snap_create, const std::string& name);

	void scan_software_by_sys(game_container& gac, const std::string& software, const std::string& parent);
	void scan_software(game_container& gac, const game_set& gar);
	void scan_alias(game_set& gar, game_container& gac, const std::string& cfg);
public:
	dmess(const std::string& Aname, const std::string& Aexe_path, const std::string& Acmd_arg);

	virtual bool run(const game& g, const game* bios, unsigned orientation, bool set_difficulty, difficulty_t difficulty, bool set_attenuation, int attenuation, bool ignore_error) const;
	virtual bool load_cfg(const game_set& gar, bool quiet);
	virtual bool load_data(const game_set& gar);
	virtual bool load_software(game_set& gar, bool quiet);

	virtual std::string type_get() const;
};

class raine_info : public emulator {
protected:
	tristate_t exclude_clone_effective;
	tristate_t exclude_clone_orig;
	tristate_t exclude_bad_effective;
	tristate_t exclude_bad_orig;
	tristate_t exclude_vertical_effective;
	tristate_t exclude_vertical_orig;

	bool load_info(game_set& gar);
public:
	raine_info(const std::string& Aname, const std::string& Aexe_path, const std::string& Acmd_arg);

	virtual int attrib_run(int x, int y);
	virtual void attrib_load();
	virtual void attrib_save();
	virtual bool attrib_set(const std::string& value0, const std::string& value1);
	virtual void attrib_get(adv_conf* config_context, const char* section, const char* tag);
	virtual bool filter(const game& g) const;
	virtual void cache(const game_set& gar, const game& g) const;
	virtual bool tree_get() const;

	virtual bool load_data(const game_set& gar);
	virtual bool load_game(game_set& gar, bool quiet);
	virtual bool load_software(game_set& gar, bool quiet);
};

class draine : public raine_info {
public:
	draine(const std::string& Aname, const std::string& Aexe_path, const std::string& Acmd_arg);

	virtual bool run(const game& g, const game* bios, unsigned orientation, bool set_difficulty, difficulty_t difficulty, bool set_attenuation, int attenuation, bool ignore_error) const;
	virtual bool load_cfg(const game_set& gar, bool quiet);

	virtual std::string type_get() const;
};

class generic : public emulator {
	bool load_info(game_set& gar);
public:
	generic(const std::string& Aname, const std::string& Aexe_path, const std::string& Acmd_arg);

	virtual int attrib_run(int x, int y);
	virtual bool tree_get() const;

	virtual bool load_cfg(const game_set& gar, bool quiet);
	virtual bool load_data(const game_set& gar);
	virtual bool load_game(game_set& gar, bool quiet);
	virtual bool load_software(game_set& gar, bool quiet);

	virtual std::string type_get() const;

	virtual bool run(const game& g, const game* bios, unsigned orientation, bool set_difficulty, difficulty_t difficulty, bool set_attenuation, int attenuation, bool ignore_error) const;
	virtual bool is_present() const;
	virtual bool is_runnable() const;
	virtual bool is_empty() const;
};

typedef std::list<emulator*> pemulator_container;
typedef std::list<std::string> emulator_container;

inline bool pgame_by_time_less(const game* A, const game* B)
{
	unsigned vA;
	unsigned vB;
	if (A->emulator_get()->tree_get())
		vA = A->time_tree_get();
	else
		vA = A->time_get();
	if (B->emulator_get()->tree_get())
		vB = B->time_tree_get();
	else
		vB = B->time_get();
	return vA > vB;
}

inline bool pgame_by_session_less(const game* A, const game* B)
{
	unsigned vA;
	unsigned vB;
	if (A->emulator_get()->tree_get())
		vA = A->session_tree_get();
	else
		vA = A->session_get();
	if (B->emulator_get()->tree_get())
		vB = B->session_tree_get();
	else
		vB = B->session_get();
	return vA > vB;
}

inline bool pgame_by_timepersession_less(const game* A, const game* B)
{
	unsigned tA;
	unsigned tB;
	unsigned cA;
	unsigned cB;
	unsigned vA;
	unsigned vB;
	if (A->emulator_get()->tree_get()) {
		tA = A->time_tree_get();
		cA = A->session_tree_get();
	} else {
		tA = A->time_get();
		cA = A->session_get();
	}
	if (B->emulator_get()->tree_get()) {
		tB = B->time_tree_get();
		cB = B->session_tree_get();
	} else {
		tB = B->time_get();
		cB = B->session_get();
	}

	if (cA)
		vA = tA / cA;
	else
		vA = 0;
	if (cB)
		vB = tB / cB;
	else
		vB = 0;

	return vA > vB;
}


#endif
