#include <thread>
#include <functional>
#include <getopt.h>
#include <string>
#include <memory>

#include "ew/core/threading/condition_variable.hpp"
#include "ew/core/program/environment.hpp"
#include "ew/utils/utils.hpp"
#include "ew/core/time/time.hpp"
using namespace ew::core::time;

#include <ew/core/dll/dll.hpp>

#include <ew/core/program/environment.hpp>

#include "ew/graphics/gui/event/push.hpp"

#include "ew/graphics/gui/display/display.hpp"


#include <ew/filesystem/mapped_file.hpp>
using namespace ew::filesystem;


/////////
#include "application.hpp"

#include "../core/core.hpp"
#include "../core/message_queue.hpp"

#include "editor_buffer.h"

//
#include "../api/include/module.h"

// default
#include "../ui/ew/ui.h"
#include "../ui/debug/ui.h"
#include "../ui/ncurses/ui.h"

#include <ew/console/console.hpp>
using namespace ew::console; // for dbg


namespace eedit
{

///////////////////////////////////////////////////////////////////////////////////////////////////

//application_module_internal.h

struct module_info_s {
	std::string  name;
	std::string  version;
	uint32_t     type; //

	std::unique_ptr<ew::core::dll> lib;

	// flags
	bool init_ok = false;
	bool deps_ok = false;
	//    lib + deps_ok + init_ok
};


class application::application_private
{
public:
	application_private();

	///
	/// \brief setup_modules load external modules defined by modules_path (std::map<key,val> -> config ?))
	/// \return true if successful otherwise false
	///
	bool setup_modules();

	///
	/// \brief release_modules unload all external modules configured by setup_modules
	/// \return true if successful otherwise false
	///
	bool release_modules();

	///
	/// \brief setup_buffers open files/buffers and bind them in the editor
	/// \return true if successful otherwise false
	///
	bool setup_buffers();

	///
	/// \brief release_buffers release all files/buffers configured by setup_buffers()
	/// \return true if successful otherwise false
	///
	bool release_buffers();

	bool main_loop();

public:
	bool debug                 = false;
	bool offscreen_buffer_flag = false;
	bool build_index_flag      = true;
	u32  nrthreads             = 0;

	mutex nr_running_threads_mtx;
	u32   nr_running_threads = 0;

	std::string ui_name;
	std::string m_font_file_name;

	std::unique_ptr<user_interface> ui;

	std::unique_ptr<ew::graphics::gui::display> gui_dpy;
	mutex                app_quit_mutex;

	std::unique_ptr<condition_variable> app_quit_condvar;


	// move to user configuration: init file
	u32 DEFAULT_FONT_SIZE   = 12;
	u32 font_width = DEFAULT_FONT_SIZE;
	u32 font_height = DEFAULT_FONT_SIZE;

	std::shared_ptr<ew::graphics::fonts::font> font;

	std::list<std::string> m_files_list;
	std::list<editor_buffer_id_t> m_buffer_desc_list;
public:
	bool quit = false;
	std::unique_ptr<std::thread> core_thread;

public:
	std::map<std::string, std::unique_ptr<module_info_s>> module_map;
};

application::application_private::  application_private()

{
	app_log << __PRETTY_FUNCTION__ << " ENTER\n";

	app_quit_condvar = std::make_unique<ew::core::threading::condition_variable>(&app_quit_mutex);

	app_log << __PRETTY_FUNCTION__ << " LEAVE\n";
}


////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<application> app;

std::shared_ptr<application> get_application()
{
	return app;
}

void set_application(std::shared_ptr<application> app_)
{
	app = app_;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

application::application()
	:
	m_priv(std::make_unique<application_private>())
{
	app_log << __PRETTY_FUNCTION__ << "\n";
}


///////////////////////////////////////////////////////////////////////////////////////////////////


void application::set_default_font_size(u32 sz)
{
	m_priv->DEFAULT_FONT_SIZE   = sz;
	m_priv->font_width = sz;
	m_priv->font_height = sz;
}

bool application::set_ui_name(const char * ui_name)
{
	auto & name = m_priv->ui_name;

	name = ui_name;

	if (name == "ew")
		return true;

	if (name == "debug")
		return true;

	if (name == "ncurses")
		return true;

	return false;
}

const std::string & application::ui_name() const
{
	return  m_priv->ui_name;
}


void application::ui(std::unique_ptr<user_interface> ui)
{
	m_priv->ui = std::move(ui);
}


user_interface * application::ui() const
{
	return m_priv->ui.get();
}



///////////////////////////////////////////////////////////////////////////////////////////////////

// - parse command line
//    build option list
//    build file list + file options

bool application::parse_command_line(int ac, char ** av)
{
	// --- default ---
	char * home_dir = ew::core::program::getenv("HOME");
	if (!home_dir) {
		cerr << "HOME env is not set\n";
		return false;
	}

	const char * default_font_sufix = ".ew/eedit/config/fonts/default.ttf";
	int len = strlen(home_dir) + 1 /* / */ + strlen(default_font_sufix);

	char * font_file_name = new char [len + 1];
	snprintf(font_file_name, len + 1, "%s/%s", home_dir, default_font_sufix);
	get_application()->set_font_file_name(font_file_name);

	delete [] font_file_name;

	// bool check_file(filename, exists|readable|...)
	{
		file * fontfile = new file(get_application()->font_file_name().c_str());
		bool ret = fontfile->open(mode::read_only);
		if (ret == false) {
			cerr << "cannot open = '" << fontfile->name() << "'\n";
			exit(0);
		}
		fontfile->close();
		delete fontfile;
	}

	// --- parse command line ---
	bool   print_help = false;
	int c;
	while (1) {
		static struct option long_options[] = {
			{"help",       no_argument,       0,  1 },
			{"offscreen",  required_argument, 0,  2 },
			{"font.size",  required_argument, 0,  3 },
			{"line.num",   required_argument, 0,  4 },
			{"ui",         required_argument, 0,  5 },
			{0,            0,                 0,  0 },
		};

		int option_index;
		c = getopt_long(ac, av, "", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 1:
			print_help = true;
			break;

		case 2:
			get_application()->offscreen_buffer_flag() = (optarg[0] == 'y' ? true : false);
			break;

		case 3:
			this->set_default_font_size(atoi(optarg));
			cerr << "font_size_x = " << this->font_width() << "\n";
			break;

		case 4:
			this->build_index_flag() = (optarg[0] == 'y' ? true : false);
			break;

		case 5:
			cerr << "detected ui = " << optarg << "\n";
			if (this->set_ui_name(optarg) != true) {
				print_help = true;
			}
			break;

		case '?':
			break;

		default: {
			// printf("?? getopt returned character code 0%o ??\n", c);
		}
		break;
		}
	}

	if (print_help) {

		cerr << "usage : eedit [ --help ]"
		     << " [ --ui ew|ncurses|debug ]"
		     << " [ --offscreen y/n]"
		     << " [ --font.size size ]"
		     << " [ --line.num y/n ]"
		     << " file ...\n";

		exit(0);
	}

	if (optind < ac) {
		while (optind < ac) {
			cerr << "add to file list '" << av[optind] << "'\n";
			files_list().push_back(av[optind++]);
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void application::set_font_file_name(char * font_file_name)
{
	m_priv->m_font_file_name = font_file_name;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

const std::string & application::font_file_name() const
{
	return m_priv->m_font_file_name;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

u32 application::font_width()
{
	return m_priv->font_width;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

u32 application::font_height()
{
	return m_priv->font_height;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool & application::offscreen_buffer_flag()
{
	return m_priv->offscreen_buffer_flag;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

bool & application::build_index_flag()
{
	return m_priv->build_index_flag;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

std::list<std::string> & application::files_list()
{
	return m_priv->m_files_list;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

application::~application()
{
}


const std::list< editor_buffer_id_t> &  application::buffer_desc_lst()
{
	return m_priv->m_buffer_desc_list;
}

std::pair<bool, editor_buffer_id_t> application::get_editor_buffer_by_byte_buffer_id(const byte_buffer_id_t id)
{
	// TOOD: use find/ array[id]
	for (auto editor_buffer_id : m_priv->m_buffer_desc_list) {
		auto cur_bid = editor_buffer_get_byte_buffer_id(editor_buffer_id);
		if (cur_bid == id) {
			return std::pair<bool, editor_buffer_id_t>(true, editor_buffer_id);
		}
	}

	return std::pair<bool, editor_buffer_id_t>(false, editor_buffer_id_t(0));
}


///////////////////////////////////////////////////////////////////////////////////////////////////


std::vector<std::string> split(std::string str, const char c)
{
	size_t start = 0;
	size_t end;

	std::vector<std::string> res;

	while (start < str.size()) {
		end = std::min(str.find_first_of(',', start), str.size());
		res.emplace_back(str.substr(start, end-start));
		start = end + 1;
	}

	return std::move(res);
}


bool application::application_private::setup_modules()
{
	// dll

	// if module path is defined ...

	// TODO: add install path in

	// must provide default buildtin modules, ascii/utf8/basic_marks
	char * mod_env = ew::core::program::getenv("EEDIT_MODULES_LIST"); // TODO: add documentation comma separated list
	if (!mod_env) {
		app_log << "env[EEDIT_MODULES_LIST] is not defined...\n";
		return true;
	}

	app_log << "env[EEDIT_MODULES_LIST] is set to : '" << mod_env << "'\n";

	std::vector<std::string> mod_vec = split(mod_env, ',');

	// module_type(codec, filter, ???)
	// extern "C" dll::module_name() -> char *
	// extern "C" dll::module_type() -> int
	// based on type the module provides other interface see codec.h  , filter.h


	for (auto & libname : mod_vec) {

		auto lib = std::make_unique<ew::core::dll>(libname.c_str());
		app_log << "try to load file '" << libname << "'\n";
		if (lib->load() == false) {
			app_log << "cannot load file '" << libname << "'\n";
			continue;
		}

		bool error = false;
		// TODO lambda over mandatory symbols array
		// extract
		const char * mandatory_symbols[] = { "module_name", "module_version", "module_type", "module_init", "module_quit", "module_depends" };
		for (auto sym : mandatory_symbols) {
			auto ptr = lib->symbol_by_name(sym);
			if (ptr == nullptr) {
				error = true;
				break;
			}
			app_log << "found symbol '" << sym << "'\n";

		}

		if (error == true) {
			continue;
		}

		app_log << libname << " : all mandatory symbols found\n";


		const char * (*modname_fn_ptr)()    = reinterpret_cast<const char * (*)()>(lib->symbol_by_name("module_name"));
		const char * (*modversion_fn_ptr)() = reinterpret_cast<const char * (*)()>(lib->symbol_by_name("module_version"));
		eedit_module_type_e  (*modtype_fn_ptr)()      = reinterpret_cast<eedit_module_type_e (*)()>(lib->symbol_by_name("module_type"));

		// add to application module map
		std::string modname           = modname_fn_ptr();
		std::string modversion        = modversion_fn_ptr();
		eedit_module_type_e  modtype = modtype_fn_ptr();


		app_log << libname << " : modname    : '" << modname << "'\n";
		app_log << libname << " : modversion : '" << modversion << "'\n";
		app_log << libname << " : modtype    : '" << modtype << "'\n";

		if (modtype == MODULE_TYPE_INVALID) {
			error = true;
			app_log << libname << " : invliad modtype\n";
		}

		if (error == false) {

			if (module_map.find(modname) != module_map.end()) {
				error = true;
			}
		}

		if (error == false) {

			eedit_module_init_status_e  (*modinit_fn_ptr)()      = reinterpret_cast<eedit_module_init_status_e (*)()>(lib->symbol_by_name("module_init"));

			eedit_module_init_status_e modinit_ret = modinit_fn_ptr();
			if (modinit_ret != MODULE_INIT_OK) {
				error = true;
				app_log << libname << " : modinit error\n";
			} else {
				app_log << libname << " : modinit ok\n";
			}
		}

		if (error == false) {

			//  allow multiple version at same time ?
			// "modname:version"

			std::string prefix;

			auto modinfo = std::make_unique<module_info_s>();

			modinfo->lib = std::move(lib);

			switch (modtype) {
			case MODULE_TYPE_CODEC:
				prefix = "codec";
				break;
			case MODULE_TYPE_FILTER:
				prefix = "filter";
				break;
			case MODULE_TYPE_EDITOR_MODE:
				prefix = "mode";
				break;
			default:
				error = true;
			}

			if (error == false) {
				module_map[prefix + ":" + modname] = std::move(modinfo);
			}

		}

	}

	for ( const auto & e : module_map) {
		app_log << e.first << "\n";
	}

	// TODO: config:

	// TODO: add config function function
	// load-codec [name]
	// load-codec text/ascii


	return true;
}

bool application::application_private::release_modules()
{
	for ( const auto & e : module_map) {
		app_log << __FUNCTION__ << " " << e.first << "\n";
	}

	return true;
}


/*
 this function create buffers from file list
 - generates buffer id
 - load text indexers

 TODO: like emacs:
  - scratch buffer
  - message buffer etc...
*/
bool application::application_private::setup_buffers()
{
	// TODO: font-family: normal/bold/italic/italic-bold
	// setup_font()
	// open fonts here
	app_log << " setup fonts\n";

	// replace : ft = font_family->normal()
	// have ft = ft->italic(1|0)->bold(1|0)

	auto ft = std::make_shared<ew::graphics::fonts::font>(get_application()->font_file_name().c_str(),
			get_application()->font_width(),
			get_application()->font_height());
	this->font = ft;
	if (ft->open() == false) {
		assert(0);
		return false;
	}

	// setup buffers
	for (const auto & filename : m_files_list) {

		// TODO: file path check/line/column

		// move this to editor_buffer_info ctor

		editor_buffer_id_t editor_buffer_id = editor_buffer_open(filename.c_str(), filename.c_str() /* buffer name */); // TODO "basename - [N]"
		if (editor_buffer_id == INVALID_EDITOR_BUFFER_ID) {
			assert(0);
			continue;
		}

		auto bid = editor_buffer_get_byte_buffer_id(editor_buffer_id);
		app_log << "allocated  bid['"<< filename  <<"'] = " << bid << "\n";
		app_log << "allocated editor_buffer_id['"<< filename  <<"'] = " << editor_buffer_id << "\n";

		if (editor_buffer_id == 0) {
			app_log << " cannot prepare buffer for '" << filename << "'\n";
			assert(0);
			continue;
		}

		app_log << " prepare buffer for '" << filename << "'\n";
		m_buffer_desc_list.emplace_back(editor_buffer_id);
	}

	// FIXME:
	// init log/message/scratch
	if (m_buffer_desc_list.size() == 0) {

		// TODO: file path check/line/column

		// move this to editor_buffer_info ctor
		auto filename = "dev/null";
		auto buffer_name = "dummy";

		editor_buffer_id_t editor_buffer_id = editor_buffer_open(filename, buffer_name); // TODO "basename - [N]"
		if (editor_buffer_id == INVALID_EDITOR_BUFFER_ID) {
			assert(0);
			return false;
		}

		auto bid = editor_buffer_get_byte_buffer_id(editor_buffer_id);
		app_log << "allocated  bid['"<< filename  <<"'] = " << bid << "\n";
		app_log << "allocated editor_buffer_id['"<< filename  <<"'] = " << editor_buffer_id << "\n";

		if (editor_buffer_id == 0) {
			app_log << " cannot prepare buffer for '" << filename << "'\n";
			assert(0);
			return false;
		}





	}

	return true;
}

bool application::application_private::release_buffers()
{
	return true;
}



///////////////////////////////////////////////////////////////////////////////////////////////////

/* TODO: the application

   will take the window as parameter
   this will allow
   ew-window
   qt-window
   ncurses-window

   must provide an abstract editor-window class ...

 */
bool application::application_private::main_loop()
{
	return ui->main_loop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool application::quit() const
{
	return m_priv->quit;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool init_ui(application * app)
{
	// init ui
	if (app->ui_name() == "debug") {
		app->ui( std::unique_ptr<user_interface>(eedit::new_debug_ui()));
		bool ret = app->ui()->setup(app);
		if (ret == false) {
			dbg << "debug ui  init() :: error" << "\n";
			return false;
		}

		return true;
	}

	if (app->ui_name() == "ncurses") {
		app->ui( std::unique_ptr<user_interface>(eedit::new_ncurses_ui()));
		bool ret = app->ui()->setup(app);
		if (ret == false) {
			dbg << "ncurses ui  init() :: error" << "\n";
			return false;
		}

		return true;
	}

	// fallback to ew
	app->set_ui_name("ew");
	app->ui(std::unique_ptr<user_interface>(eedit::new_ew_ui()));
	bool ret = app->ui()->setup(app);
	if (ret == false) {
		dbg << "ew ui  init() :: error" << "\n";
		return false;
	}

	return true;
}

bool init_subsystems(application * app)
{
	if (ew::core::time::init() == false) {
		dbg << "ew::core::time::init() :: error" << "\n";
		exit(1);
	}

	// init ui
	if (init_ui(app) == false) {
		dbg << "cannot ew::graphics::gui::init() :: error" << "\n";
		exit(1);
	}

	if (ew::graphics::fonts::init() == false) {
		dbg << "ew::graphics::fonts::init() :: error" << "\n";
		exit(1);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////

bool quit_ui(application * app)
{
	return true;
}


bool  quit_subsystems(application * app)
{
	quit_ui(app);

	if (ew::graphics::fonts::quit() == false) {
		dbg << "ew::graphics::fonts::quit() :: error" << "\n";
		return false;
	}

	if (ew::graphics::rendering::quit() == false) {
		dbg << "ew::graphics::gui::quit() :: error" << "\n";
		return false;
	}

	if (ew::core::time::quit() == false) {
		dbg << "ew::core::time::quit() :: error" << "\n";
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////

/* FIXME:
 * - parse options first
 * - load configuration
 * - select/init gui (no window opened yet)
 * - init buffers / load files
 * - start event loop etc ...
 * - show main window
 */
bool application::run(int ac, char ** av)
{
	bool ret;

	//
	ret = ew::codecs::text::unicode::utf8::init();
	if (ret == false) {
		return false;
	}

	// parse command line
	ret = parse_command_line(ac, av);
	if (ret == false) {
		return false;
	}

	ret = init_subsystems(this);
	if (ret == false) {
		return false;
	}


	// prepare buffers, files, fonts etc ..
	ret = m_priv->setup_modules();

	// prepare buffers, files, fonts etc ..
	ret = m_priv->setup_buffers();

	// start core thread
	m_priv->core_thread = std::make_unique<std::thread>(eedit::core::main, app);

	// ui main loop
	ret = m_priv->main_loop();

	// wait for core completion
	m_priv->core_thread->join();

	// prepare buffers, files, fonts etc ..
	ret = m_priv->release_buffers();

	//
	ret = m_priv->release_modules();

	quit_subsystems(this);

	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////




} // ! namespace eedit
