/*
* TeamSpeak 3 demo plugin
*
* Copyright (c) 2008-2017 TeamSpeak Systems GmbH
*/

#ifdef _WIN32
#pragma warning (disable : 4100)  /* Disable Unreferenced parameter warning */
#include <Windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"
#include "teamspeak/public_definitions.h"
#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/clientlib_publicdefinitions.h"
#include "ts3_functions.h"
#include "plugin.h"
#include <iostream>
#include <string>
#include "QtGui.h"
#include "config.h"

static struct TS3Functions ts3Functions;

#ifdef _WIN32
#define _strcpy(dest, destSize, src) strcpy_s(dest, destSize, src)
#define snprintf sprintf_s
#else
#define _strcpy(dest, destSize, src) { strncpy(dest, src, destSize-1); (dest)[destSize-1] = '\0'; }
#endif

#define PLUGIN_API_VERSION 22

#define PATH_BUFSIZE 512
#define COMMAND_BUFSIZE 128
#define INFODATA_BUFSIZE 128
#define SERVERINFO_BUFSIZE 256
#define CHANNELINFO_BUFSIZE 512
#define RETURNCODE_BUFSIZE 128

#define PLUGIN_NAME "Auto Answer Block"
//Global Variables
static char* pluginID = NULL;
std::string configFile;

#ifdef _WIN32
/* Helper function to convert wchar_T to Utf-8 encoded strings on Windows */
static int wcharToUtf8(const wchar_t* str, char** result) {
	int outlen = WideCharToMultiByte(CP_UTF8, 0, str, -1, 0, 0, 0, 0);
	*result = (char*)malloc(outlen);
	if (WideCharToMultiByte(CP_UTF8, 0, str, -1, *result, outlen, 0, 0) == 0) {
		*result = NULL;
		return -1;
	}
	return 0;
}
#endif

/*********************************** Required functions ************************************/


const char* ts3plugin_name() {
#ifdef _WIN32
	static char* result = NULL;  /* Static variable so it's allocated only once */
	if (!result) {
		const wchar_t* name = L"Auto Answer Block";
		if (wcharToUtf8(name, &result) == -1) {  /* Convert name into UTF-8 encoded result */
			result = PLUGIN_NAME;  /* Conversion failed, fallback here */
		}
	}
	return result;
#else
	return PLUGIN_NAME;
#endif
}

const char* ts3plugin_version() {
	return "1.0";
}

int ts3plugin_apiVersion() {
	return PLUGIN_API_VERSION;
}

const char* ts3plugin_author() {
	return "Exp";
}

const char* ts3plugin_description() {
	return "Block automated text messages/pokes from other clients.";
}

void ts3plugin_setFunctionPointers(const struct TS3Functions funcs) {
	ts3Functions = funcs;
}

int ts3plugin_init() {
	char appPath[PATH_BUFSIZE];
	char resourcesPath[PATH_BUFSIZE];
	char configPath[PATH_BUFSIZE];
	char pluginPath[PATH_BUFSIZE];

	ts3Functions.getAppPath(appPath, PATH_BUFSIZE);
	ts3Functions.getResourcesPath(resourcesPath, PATH_BUFSIZE);
	ts3Functions.getConfigPath(configPath, PATH_BUFSIZE);
	ts3Functions.getPluginPath(pluginPath, PATH_BUFSIZE, pluginID);

	return 0;
}

void ts3plugin_shutdown() {
	if (pluginID) {
		free(pluginID);
		pluginID = NULL;
	}
}

void openGui(void* qParentWidget = nullptr)
{
	QtGui* cfg = new QtGui((QWidget*)qParentWidget);
	cfg->setAttribute(Qt::WA_DeleteOnClose);
	cfg->show();
}

int ts3plugin_offersConfigure() {
	return PLUGIN_OFFERS_CONFIGURE_QT_THREAD;
}

void ts3plugin_configure(void* handle, void* qParentWidget)
{
	openGui(qParentWidget);
}

void ts3plugin_registerPluginID(const char* id) {
	const size_t sz = strlen(id) + 1;
	pluginID = (char*)malloc(sz * sizeof(char));
	_strcpy(pluginID, sz, id);
}

const char* ts3plugin_commandKeyword() {
	return ""; //Command Suffix for chat; example: /test
}

const char* ts3plugin_infoTitle() {
	return PLUGIN_NAME;
}

void ts3plugin_freeMemory(void* data) {
	free(data);
}
int ts3plugin_requestAutoload() {
	return 0;
}

/* Helper function to create a menu item */
static struct PluginMenuItem* createMenuItem(enum PluginMenuType type, int id, const char* text, const char* icon) {
	struct PluginMenuItem* menuItem = (struct PluginMenuItem*)malloc(sizeof(struct PluginMenuItem));
	menuItem->type = type;
	menuItem->id = id;
	_strcpy(menuItem->text, PLUGIN_MENU_BUFSZ, text);
	_strcpy(menuItem->icon, PLUGIN_MENU_BUFSZ, icon);
	return menuItem;
}

/* Some makros to make the code to create menu items a bit more readable */
#define BEGIN_CREATE_MENUS(x) const size_t sz = x + 1; size_t n = 0; *menuItems = (struct PluginMenuItem**)malloc(sizeof(struct PluginMenuItem*) * sz);
#define CREATE_MENU_ITEM(a, b, c, d) (*menuItems)[n++] = createMenuItem(a, b, c, d);
#define END_CREATE_MENUS (*menuItems)[n++] = NULL; assert(n == sz);


enum {
	MENU_ID_CLIENT_1 = 1,
	MENU_ID_MAX
};

void ts3plugin_initMenus(struct PluginMenuItem*** menuItems, char** menuIcon) {

	BEGIN_CREATE_MENUS(MENU_ID_MAX - 1); //Needs to be correct
	CREATE_MENU_ITEM(PLUGIN_MENU_TYPE_CLIENT, MENU_ID_CLIENT_1, "Toggle blocker for this client", "");
	END_CREATE_MENUS;

	*menuIcon = (char*)malloc(PLUGIN_MENU_BUFSZ * sizeof(char));
	_strcpy(*menuIcon, PLUGIN_MENU_BUFSZ, ""); //PLUGIN MENU IMAGE
}

/************************** TeamSpeak callbacks ***************************/

void ts3plugin_onMenuItemEvent(uint64 serverConnectionHandlerID, enum PluginMenuType type, int menuItemID, uint64 selectedItemID) {
	switch (type) {
	case PLUGIN_MENU_TYPE_CLIENT:
		/* Channel contextmenu item was triggered. selectedItemID is the ID of the selected client */
		switch (menuItemID) {
		case MENU_ID_CLIENT_1:
		{
			char* uid = new char[256];
			if (ts3Functions.getClientVariableAsString(serverConnectionHandlerID, selectedItemID, ClientProperties::CLIENT_UNIQUE_IDENTIFIER, &uid) == ERROR_ok)
			{
				std::string id = uid;

				std::string msg;

				auto find = std::find(config->whiteListUIDs.begin(), config->whiteListUIDs.end(), id);
				if (find != config->whiteListUIDs.end())
				{
					config->whiteListUIDs.erase(config->whiteListUIDs.begin() + distance(config->whiteListUIDs.begin(), find));
					msg = "Removed " + id + " from the whitelist.";
				}
				else
				{
					config->whiteListUIDs.push_back(id);
					msg = "Added " + id + " to the whitelist.";
				}
				ts3Functions.printMessageToCurrentTab(msg.c_str());
			}
			break;
		}
		default:
			break;
		}
		break;
	default:
		break;
	}
}

const char* ts3plugin_keyDeviceName(const char* keyIdentifier) {
	return NULL;
}

const char* ts3plugin_displayKeyText(const char* keyIdentifier) {
	return NULL;
}

const char* ts3plugin_keyPrefix() {
	return NULL;
}

double GetEpochTimeInMilliseconds()
{
	double now = (double)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	return now;
}

int checkMessage(const char* identity)
{
	double now = GetEpochTimeInMilliseconds();
	
	bool isWhitelisted = (std::find(config->whiteListUIDs.begin(), config->whiteListUIDs.end(), std::string(identity)) == config->whiteListUIDs.end());
	int result = 0;
	if (config->lastTimestamp != -1 && isWhitelisted) //first message
	{
		if (config->lastTimestamp > (now - config->delay)) //too fast
		{
			result = 1;
		}
	}

	//Set timestamp to now
	config->lastTimestamp = now;
	
	return result; //0 = don't block; 1 = block
}

int ts3plugin_onTextMessageEvent(uint64 serverConnectionHandlerID, anyID targetMode, anyID toID, anyID fromID, const char * fromName, const char * fromUniqueIdentifier, const char * message, int ffIgnored)
{
	int result = checkMessage(fromUniqueIdentifier);
	return result;
}

int ts3plugin_onClientPokeEvent(uint64 serverConnectionHandlerID, anyID fromClientID, const char* pokerName, const char* pokerUniqueIdentity, const char* message, int ffIgnored)
{
	int result = checkMessage(pokerUniqueIdentity);
	return result;
}