#include "gui_main.hpp"
#include "button.hpp"

#include <stdio.h>
#include <dirent.h>
#include <sstream>

#include "list_selector.hpp"

static std::vector<std::string> autobootNames;
static u16 currAutoBootEntryIndex;
static u16 currOverrideTidIndex;

GuiMain::GuiMain() : Gui() {
	initLoader(&m_overrideAllAppByDefault, &m_overrideKeyCombo, &m_overrideByDefault, &m_overrideMITMKeyCombo, &m_overrideMITMByDefault, &m_overrideCheatKeyCombo, &m_overrideCheatByDefault);
	initSystemSettings();
	// TODO - Fill this in as you go
	printf("%lx\n", m_overrideKeyCombo);

	m_currAutoBootConfig = getAutoBootConfigs(m_autoBootConfigs, currAutoBootEntryIndex);

	// Button 0 - HBMenu key combo button
	new Button(40, 100, 170, 170, [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
		gui->drawTextAligned(fontHuge, x + 23, y + 123, currTheme.textColor, keyToUnicode(m_overrideKeyCombo), ALIGNED_LEFT);
		gui->drawTextAligned(font14, x + 87, y + 162, currTheme.textColor, "HBMenu Key", ALIGNED_CENTER);
	}, [&](u32 kdown, bool *isActivated) {
		if (*isActivated) {
			if (!(kdown & (kdown - 1)) && (kdown <= KEY_DDOWN || kdown >= KEY_SL) && kdown != KEY_TOUCH) {
				m_overrideKeyCombo = kdown;
				Ini *ini = Ini::parseFile(LOADER_INI);
				auto ini_override_key = ini->findSection("hbl_config")->findFirstOption("override_key");
				ini_override_key->value = GuiMain::keyToKeyChars(m_overrideKeyCombo, m_overrideByDefault);

				ini->writeToFile(LOADER_INI);
				*isActivated = false;

				delete ini;
			}
		}
	}, { -1, 4, -1, 1 }, true);
	// {Up->Button#, Down->Button#, Left->Button#, Right->Button#}, Interactable

	// Button 1 - Open HBMenu by default button
	new Button(220, 100, 505, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
		gui->drawTextAligned(font20, x + 20, y + 50, currTheme.textColor, "Open HBMenu by default", ALIGNED_LEFT);
		gui->drawTextAligned(font20, x + 435, y + 50, m_overrideByDefault ? currTheme.selectedColor : Gui::makeColor(0xB8, 0xBB, 0xC2, 0xFF), m_overrideByDefault ? "On" : "Off", ALIGNED_LEFT);
	}, [&](u32 kdown, bool *isActivated) {
		if (kdown & KEY_A) {
			m_overrideByDefault = !m_overrideByDefault;
			Ini *ini = Ini::parseFile(LOADER_INI);
			auto ini_override_key = ini->findSection("hbl_config")->findFirstOption("override_key");
			ini_override_key->value = GuiMain::keyToKeyChars(m_overrideKeyCombo, m_overrideByDefault);

			ini->writeToFile(LOADER_INI);
			delete ini;
		}
	}, { -1, 2, 0, 7 }, false);

	// Button 2 - Open HBMenu with any app
	new Button(220, 190, 505, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
		gui->drawTextAligned(font20, x + 20, y + 50, currTheme.textColor, "Open HBMenu with any app", ALIGNED_LEFT);
		gui->drawTextAligned(font20, x + 435, y + 50, m_overrideAllAppByDefault ? currTheme.selectedColor : Gui::makeColor(0xB8, 0xBB, 0xC2, 0xFF), m_overrideAllAppByDefault ? "On" : "Off", ALIGNED_LEFT);
	}, [&](u32 kdown, bool *isActivated) {
		if (kdown & KEY_A) {
			m_overrideAllAppByDefault = !m_overrideAllAppByDefault;
			Ini *ini = Ini::parseFile(LOADER_INI);
			auto ini_override_any_app = ini->findSection("hbl_config")->findFirstOption("override_any_app");
			ini_override_any_app->value = m_overrideAllAppByDefault ? "true" : "false";

			ini->writeToFile(LOADER_INI);
			delete ini;
		}
	}, { 1, 3, 0, 8 }, false);

	// Button 3 - AMS-Mitm by default button
	new Button(220, 280, 505, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
		gui->drawTextAligned(font20, x + 20, y + 50, currTheme.textColor, "Enable AMS-Mitm by default", ALIGNED_LEFT);
		gui->drawTextAligned(font20, x + 435, y + 50, m_overrideMITMByDefault ? currTheme.selectedColor : Gui::makeColor(0xB8, 0xBB, 0xC2, 0xFF), m_overrideMITMByDefault ? "On" : "Off", ALIGNED_LEFT);
	}, [&](u32 kdown, bool *isActivated) {
		if (kdown & KEY_A) {
			m_overrideMITMByDefault = !m_overrideMITMByDefault;
			Ini *ini = Ini::parseFile(LOADER_INI);
			auto ini_override_Mitm_key = ini->findSection("default_config")->findFirstOption("override_key");
			ini_override_Mitm_key->value = GuiMain::keyToKeyChars(m_overrideMITMKeyCombo, m_overrideMITMByDefault);

			ini->writeToFile(LOADER_INI);
			delete ini;
		}
	}, { 2, 5, 4, 9 }, false);

	// Button 4 - AMS-Mitm key combo button
	new Button(40, 280, 170, 170, [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
		gui->drawTextAligned(fontHuge, x + 23, y + 123, currTheme.textColor, keyToUnicode(m_overrideMITMKeyCombo), ALIGNED_LEFT);
		gui->drawTextAligned(font14, x + 87, y + 162, currTheme.textColor, "AMS-Mitm Key", ALIGNED_CENTER);
	}, [&](u32 kdown, bool *isActivated) {
		if (*isActivated) {
			if (!(kdown & (kdown - 1)) && (kdown <= KEY_DDOWN || kdown >= KEY_SL) && kdown != KEY_TOUCH) {
				m_overrideMITMKeyCombo = kdown;
				Ini *ini = Ini::parseFile(LOADER_INI);
				auto ini_override_Mitm_key = ini->findSection("default_config")->findFirstOption("override_key");
				ini_override_Mitm_key->value = GuiMain::keyToKeyChars(m_overrideMITMKeyCombo, m_overrideMITMByDefault);

				ini->writeToFile(LOADER_INI);
				*isActivated = false;

				delete ini;
			}
		}
	}, { 0, 6, -1, 3 }, true);

	// Button 5 - Cheats by default button
	new Button(220, 370, 505, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
		gui->drawTextAligned(font20, x + 20, y + 50, currTheme.textColor, "Enable Cheats by default", ALIGNED_LEFT);
		gui->drawTextAligned(font20, x + 435, y + 50, m_overrideCheatByDefault ? currTheme.selectedColor : Gui::makeColor(0xB8, 0xBB, 0xC2, 0xFF), m_overrideCheatByDefault ? "On" : "Off", ALIGNED_LEFT);
	}, [&](u32 kdown, bool *isActivated) {
		if (kdown & KEY_A) {
			m_overrideCheatByDefault = !m_overrideCheatByDefault;
			Ini *ini = Ini::parseFile(LOADER_INI);
			auto ini_override_Cheat_key = ini->findSection("default_config")->findFirstOption("cheat_enable_key");
			ini_override_Cheat_key->value = GuiMain::keyToKeyChars(m_overrideCheatKeyCombo, m_overrideCheatByDefault);

			ini->writeToFile(LOADER_INI);
			delete ini;
		}
	}, { 3, -1, 4, 10 }, false);

	// Button 6 - Cheats key combo button
	new Button(40, 460, 170, 170, [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
		gui->drawTextAligned(fontHuge, x + 23, y + 123, currTheme.textColor, keyToUnicode(m_overrideCheatKeyCombo), ALIGNED_LEFT);
		gui->drawTextAligned(font14, x + 87, y + 162, currTheme.textColor, "Cheats Key", ALIGNED_CENTER);
	}, [&](u32 kdown, bool *isActivated) {
		if (*isActivated) {
			if (!(kdown & (kdown - 1)) && (kdown <= KEY_DDOWN || kdown >= KEY_SL) && kdown != KEY_TOUCH) {
				m_overrideCheatKeyCombo = kdown;
				Ini *ini = Ini::parseFile(LOADER_INI);
				auto ini_override_Cheat_key = ini->findSection("default_config")->findFirstOption("cheat_enable_key");
				ini_override_Cheat_key->value = GuiMain::keyToKeyChars(m_overrideCheatKeyCombo, m_overrideCheatByDefault);

				ini->writeToFile(LOADER_INI);
				*isActivated = false;

				delete ini;
			}
		}
	}, { 4, -1, -1, 5 }, true);

	// Button 7 - Upload error reports button
	new Button(735, 100, 505, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
		gui->drawTextAligned(font20, x + 20, y + 50, currTheme.textColor, "Upload Error Reports", ALIGNED_LEFT);
		gui->drawTextAligned(font20, x + 435, y + 50, m_overrideCheatByDefault ? currTheme.selectedColor : Gui::makeColor(0xB8, 0xBB, 0xC2, 0xFF), "--", ALIGNED_LEFT);
	}, [&](u32 kdown, bool *isActivated) {
		if (kdown & KEY_A) {

		}
	}, { -1, 8, 1, -1 }, false);

	// Button 8 - Enable USB 3.0 button
	new Button(735, 190, 505, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
		gui->drawTextAligned(font20, x + 20, y + 50, currTheme.textColor, "Enable USB 3.0", ALIGNED_LEFT);
		gui->drawTextAligned(font20, x + 435, y + 50, m_overrideCheatByDefault ? currTheme.selectedColor : Gui::makeColor(0xB8, 0xBB, 0xC2, 0xFF), "--", ALIGNED_LEFT);
	}, [&](u32 kdown, bool *isActivated) {
		if (kdown & KEY_A) {

		}
	}, { 7, 9, 2, -1 }, false);

	// Button 9 - Reboot behavior button
	new Button(735, 280, 505, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
		gui->drawTextAligned(font20, x + 20, y + 50, currTheme.textColor, "Reboot behavior", ALIGNED_LEFT);
		gui->drawTextAligned(font20, x + 435, y + 50, m_overrideCheatByDefault ? currTheme.selectedColor : Gui::makeColor(0xB8, 0xBB, 0xC2, 0xFF), "--", ALIGNED_LEFT);
	}, [&](u32 kdown, bool *isActivated) {
		if (kdown & KEY_A) {

		}
	}, { 8, 10, 3, -1 }, false);

	// Button 10 - Cheats active by defualt button
	new Button(735, 370, 505, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
		gui->drawTextAligned(font20, x + 20, y + 50, currTheme.textColor, "Cheats active by default", ALIGNED_LEFT);
		gui->drawTextAligned(font20, x + 435, y + 50, m_overrideCheatByDefault ? currTheme.selectedColor : Gui::makeColor(0xB8, 0xBB, 0xC2, 0xFF), "--", ALIGNED_LEFT);
	}, [&](u32 kdown, bool *isActivated) {
		if (kdown & KEY_A) {

		}
	}, { 9, 11, 5, -1 }, false);

	// Button 11 - Save Cheat toggles button
	new Button(735, 460, 505, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
		gui->drawTextAligned(font20, x + 20, y + 50, currTheme.textColor, "Save Cheat toggles", ALIGNED_LEFT);
		gui->drawTextAligned(font20, x + 435, y + 50, m_overrideCheatByDefault ? currTheme.selectedColor : Gui::makeColor(0xB8, 0xBB, 0xC2, 0xFF), "--", ALIGNED_LEFT);
	}, [&](u32 kdown, bool *isActivated) {
		if (kdown & KEY_A) {

		}
	}, { 10, -1, 5, -1 }, false);
}

GuiMain::~GuiMain() {
}

const char* GuiMain::keyToUnicode(u64 key) {
	switch (key) {
	case KEY_A:       return "\uE0E0";
	case KEY_B:       return "\uE0E1";
	case KEY_X:       return "\uE0E2";
	case KEY_Y:       return "\uE0E3";
	case KEY_LSTICK:  return "\uE101";
	case KEY_RSTICK:  return "\uE102";
	case KEY_L:       return "\uE0E4";
	case KEY_R:       return "\uE0E5";
	case KEY_ZL:      return "\uE0E6";
	case KEY_ZR:      return "\uE0E7";
	case KEY_PLUS:    return "\uE0EF";
	case KEY_MINUS:   return "\uE0F0";
	case KEY_DLEFT:   return "\uE0ED";
	case KEY_DUP:     return "\uE0EB";
	case KEY_DRIGHT:  return "\uE0EE";
	case KEY_DDOWN:   return "\uE0EC";
	case KEY_SL:      return "\uE0E8";
	case KEY_SR:      return "\uE0E9";
	default:          return "\uE152";
	}
}

std::string GuiMain::keyToKeyChars(u64 key, bool overrideByDefault) {
	std::string ret = overrideByDefault ? "!" : "";
	switch (key) {
	case KEY_A:       ret += "A";        break;
	case KEY_B:       ret += "B";        break;
	case KEY_X:       ret += "X";        break;
	case KEY_Y:       ret += "Y";        break;
	case KEY_LSTICK:  ret += "LS";       break;
	case KEY_RSTICK:  ret += "RS";       break;
	case KEY_L:       ret += "L";        break;
	case KEY_R:       ret += "R";        break;
	case KEY_ZL:      ret += "ZL";       break;
	case KEY_ZR:      ret += "ZR";       break;
	case KEY_PLUS:    ret += "PLUS";     break;
	case KEY_MINUS:   ret += "MINUS";    break;
	case KEY_DLEFT:   ret += "DLEFT";    break;
	case KEY_DUP:     ret += "DUP";      break;
	case KEY_DRIGHT:  ret += "DRIGHT";   break;
	case KEY_DDOWN:   ret += "DDOWN";    break;
	case KEY_SL:      ret += "SL";       break;
	case KEY_SR:      ret += "SR";       break;
	}

	return ret;
}

void GuiMain::keyCharsToKey(std::string str, u64 *key, bool *overrideByDefault) {
	*overrideByDefault = (str[0] == '!');

	if (*overrideByDefault) {
		str = str.substr(1);
	}

	if (str == "A") *key = KEY_A;
	else if (str == "B") *key = KEY_B;
	else if (str == "X") *key = KEY_X;
	else if (str == "Y") *key = KEY_Y;
	else if (str == "LS") *key = KEY_LSTICK;
	else if (str == "RS") *key = KEY_RSTICK;
	else if (str == "L") *key = KEY_L;
	else if (str == "R") *key = KEY_R;
	else if (str == "ZL") *key = KEY_ZL;
	else if (str == "ZR") *key = KEY_ZR;
	else if (str == "PLUS") *key = KEY_PLUS;
	else if (str == "MINUS") *key = KEY_MINUS;
	else if (str == "DLEFT") *key = KEY_DLEFT;
	else if (str == "DUP") *key = KEY_DUP;
	else if (str == "DRIGHT") *key = KEY_DRIGHT;
	else if (str == "DDOWN") *key = KEY_DDOWN;
	else if (str == "SL") *key = KEY_SL;
	else if (str == "SR") *key = KEY_SR;
}

void GuiMain::initLoader(bool *overrideAllAppByDefault, u64 *key, bool *overrideByDefault, u64 *MITMkey, bool *overrideMITMByDefault, u64 *Cheatkey, bool *overrideCheatByDefault) {
	Ini *ini = Ini::parseFile(LOADER_INI);
	*overrideAllAppByDefault = (ini->findSection("hbl_config")->findFirstOption("override_any_app")->value == "true");

	GuiMain::keyCharsToKey(ini->findSection("hbl_config")->findFirstOption("override_key")->value, key, overrideByDefault);
	GuiMain::keyCharsToKey(ini->findSection("default_config")->findFirstOption("override_key")->value, MITMkey, overrideMITMByDefault);
	GuiMain::keyCharsToKey(ini->findSection("default_config")->findFirstOption("cheat_enable_key")->value, Cheatkey, overrideCheatByDefault);
}

void GuiMain::initSystemSettings(void) {
	
}

AutoBootEntry GuiMain::getAutoBootConfigs(std::vector<AutoBootEntry> &out_bootEntries, u16 &currAutoBootEntryIndex) {
	Ini *hekateIni = Ini::parseFile(HEKATE_INI);

	u16 id = 0;
	AutoBootEntry currEntry;

	currAutoBootEntryIndex = 0;

	u16 currAutoboot = std::stoi(hekateIni->findSection("config")->findFirstOption("autoboot")->value);
	bool currAutoboot_list = std::stoi(hekateIni->findSection("config")->findFirstOption("autoboot_list")->value);

	out_bootEntries.push_back({ "Disable autoboot", 0, false });
	currEntry = out_bootEntries.back();

	for (auto const& it : hekateIni->sections) {
		if (std::string(it->value) == "config" || it->isComment()) continue;
		out_bootEntries.push_back({ it->value, ++id, false });

		if (!currAutoboot_list && id == currAutoboot) {
			currEntry = out_bootEntries.back();
			currAutoBootEntryIndex = out_bootEntries.size() - 1;
		}
	}

	DIR *dr = opendir(INI_PATH);
	struct dirent *de;
	std::vector<std::string> iniFiles;

	while ((de = readdir(dr)) != nullptr)
		iniFiles.push_back(de->d_name);
	closedir(dr);

	std::sort(iniFiles.begin(), iniFiles.end());

	id = 0;

	delete hekateIni;
	hekateIni = nullptr;

	if (iniFiles.empty()) return currEntry;

	for (auto const& iniFile : iniFiles) {
		std::string file = std::string(INI_PATH) + iniFile;
		hekateIni = Ini::parseFile(file);

		for (auto const& it : hekateIni->sections) {
			if (it->isComment()) continue;
			out_bootEntries.push_back({ it->value, ++id, true });

			if (currAutoboot_list && id == currAutoboot) {
				currEntry = out_bootEntries.back();
				currAutoBootEntryIndex = out_bootEntries.size() - 1;
			}
		}

		delete hekateIni;
	}

	return currEntry;
}

void GuiMain::update() {
	Gui::update();
}

void GuiMain::draw() {
	Gui::beginDraw();
	Gui::drawRectangle(0, 0, Gui::g_framebuffer_width, Gui::g_framebuffer_height, currTheme.backgroundColor);
	Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), 87, 1220, 1, currTheme.textColor);
	Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), Gui::g_framebuffer_height - 73, 1220, 1, currTheme.textColor);
	Gui::drawTextAligned(fontIcons, 50, 68, currTheme.textColor, "\uE130", ALIGNED_LEFT);
	Gui::drawTextAligned(font24, 50, 58, currTheme.textColor, "        Atmosphere Toolkit", ALIGNED_LEFT);
	Gui::drawTextAligned(font20, Gui::g_framebuffer_width - 50, 58, currTheme.textColor, "\uE0E1 Back     \uE0E0 Ok", ALIGNED_RIGHT);
	Gui::drawTextAligned(font24, Gui::g_framebuffer_width / 2, Gui::g_framebuffer_height - 28, currTheme.textColor, "Press \uE044 to save and return back to the home menu", ALIGNED_CENTER);

	for (Button *btn : Button::g_buttons)
		btn->draw(this);
	Gui::endDraw();
}

void GuiMain::onInput(u32 kdown) {
	for (Button *btn : Button::g_buttons) {
		if (btn->isSelected())
			if (btn->onInput(kdown)) break;
	}
}

void GuiMain::onTouch(touchPosition &touch) {
	for (Button *btn : Button::g_buttons) {
		btn->onTouch(touch);
	}
}

void GuiMain::onGesture(touchPosition &startPosition, touchPosition &endPosition) {

}
