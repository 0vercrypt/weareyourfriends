/*
 * TeamSpeak 3 demo plugin
 *
 * Copyright (c) 2008-2016 TeamSpeak Systems GmbH
 */

#ifdef _WIN32
#pragma warning (disable : 4100)  /* Disable Unreferenced parameter warning */
#include <Windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <assert.h>
#include <qsettings.h>
#include <qstring.h>
#include <qstandardpaths.h>
#include "wayfconfigdialog.h"
#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"
#include "teamspeak/public_definitions.h"
#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/clientlib_publicdefinitions.h"
#include "ts3_functions.h"
#include "sqlite3.h"
#include "plugin.h"

static struct TS3Functions ts3Functions;
QString WAYFSettingsFile;

#ifdef _WIN32
#define _strcpy(dest, destSize, src) strcpy_s(dest, destSize, src)
#define snprintf sprintf_s
#else
#define _strcpy(dest, destSize, src) { strncpy(dest, src, destSize-1); (dest)[destSize-1] = '\0'; }
#endif

#define PLUGIN_API_VERSION 21

#define PATH_BUFSIZE 512
#define COMMAND_BUFSIZE 128
#define INFODATA_BUFSIZE 128
#define SERVERINFO_BUFSIZE 256
#define CHANNELINFO_BUFSIZE 512
#define RETURNCODE_BUFSIZE 128

static char* pluginID = NULL;
sqlite3 *db;

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
/*
 * If any of these required functions is not implemented, TS3 will refuse to load the plugin
 */

/* Unique name identifying this plugin */
const char* ts3plugin_name() {
#ifdef _WIN32
	/* TeamSpeak expects UTF-8 encoded characters. Following demonstrates a possibility how to convert UTF-16 wchar_t into UTF-8. */
	static char* result = NULL;  /* Static variable so it's allocated only once */
	if (!result) {
		const wchar_t* name = L"We are your friends (WAYF)";
		if (wcharToUtf8(name, &result) == -1) {  /* Convert name into UTF-8 encoded result */
			result = "We are your friends (WAYF)";  /* Conversion failed, fallback here */
		}
	}
	return result;
#else
	return "We are your friends (WAYF)";
#endif
}

/* Plugin version */
const char* ts3plugin_version() {
	return "1.0";
}

/* Plugin API version. Must be the same as the clients API major version, else the plugin fails to load. */
int ts3plugin_apiVersion() {
	return PLUGIN_API_VERSION;
}

/* Plugin author */
const char* ts3plugin_author() {
	/* If you want to use wchar_t, see ts3plugin_name() on how to use */
	return "Leibi (teamspeak-plugins.org)";
}

/* Plugin description */
const char* ts3plugin_description() {
	/* If you want to use wchar_t, see ts3plugin_name() on how to use */
	return "Dieses Plugin weist Freunden und blockierten Nutzern beim Betreten des eigenen Channels eine in den Einstellungen definierte Channel Gruppe zu.";
}

/* Set TeamSpeak 3 callback functions */
void ts3plugin_setFunctionPointers(const struct TS3Functions funcs) {
	ts3Functions = funcs;
}

/*
 * Custom code called right after loading the plugin. Returns 0 on success, 1 on failure.
 * If the function returns 1 on failure, the plugin will be unloaded again.
 */
int ts3plugin_init()
{
	// load WAYS settings file path
	WAYFSettingsFile = QStandardPaths::locate(QStandardPaths::AppLocalDataLocation, QString(), QStandardPaths::LocateDirectory) + "wayf.ini";

	int rc;
	char path[255];
	strcpy(path, getenv("APPDATA"));
	strcat_s(path, "\\TS3Client\\settings.db");
	printf("path: %s\n", path);
	rc = sqlite3_open(path, &db);
	if (rc)
		printf("Can't open database.\n");
	else
		printf("DB opened!\n");

	return 0;  /* 0 = success, 1 = failure, -2 = failure but client will not show a "failed to load" warning */
	/* -2 is a very special case and should only be used if a plugin displays a dialog (e.g. overlay) asking the user to disable
	 * the plugin again, avoiding the show another dialog by the client telling the user the plugin failed to load.
	 * For normal case, if a plugin really failed to load because of an error, the correct return value is 1. */
}

/* Custom code called right before the plugin is unloaded */
void ts3plugin_shutdown() {
	/*
	 * Note:
	 * If your plugin implements a settings dialog, it must be closed and deleted here, else the
	 * TeamSpeak client will most likely crash (DLL removed but dialog from DLL code still open).
	 */
	sqlite3_close(db);
	printf("WAYF: shutdown...\n");

	/* Free pluginID if we registered it */
	if (pluginID) {
		free(pluginID);
		pluginID = NULL;
	}
}

/****************************** Optional functions ********************************/
/*
 * Following functions are optional, if not needed you don't need to implement them.
 */

/* Tell client if plugin offers a configuration window. If this function is not implemented, it's an assumed "does not offer" (PLUGIN_OFFERS_NO_CONFIGURE). */
int ts3plugin_offersConfigure() {
	return PLUGIN_OFFERS_CONFIGURE_QT_THREAD;  /* In this case ts3plugin_configure does not need to be implemented */
}

/* Plugin might offer a configuration window. If ts3plugin_offersConfigure returns 0, this function does not need to be implemented. */
void ts3plugin_configure(void* handle, void* qParentWidget) {
	WAYFConfigDialog* config = new WAYFConfigDialog((QWidget*)qParentWidget);
	config->setAttribute(Qt::WA_DeleteOnClose);
	config->setWindowFlags(config->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	if (config->isValid)
	{
		config->exec();
	}
	else
	{
		config->~WAYFConfigDialog();
	}
}

/*
 * If the plugin wants to use error return codes, plugin commands, hotkeys or menu items, it needs to register a command ID. This function will be
 * automatically called after the plugin was initialized. This function is optional. If you don't use these features, this function can be omitted.
 * Note the passed pluginID parameter is no longer valid after calling this function, so you must copy it and store it in the plugin.
 */
void ts3plugin_registerPluginID(const char* id) {
	const size_t sz = strlen(id) + 1;
	pluginID = (char*)malloc(sz * sizeof(char));
	_strcpy(pluginID, sz, id);  /* The id buffer will invalidate after exiting this function */
	printf("WAYF: registerPluginID: %s\n", pluginID);
}

/* Plugin command keyword. Return NULL or "" if not used. */
const char* ts3plugin_commandKeyword() {
	return NULL;
}

/* Plugin processes console command. Return 0 if plugin handled the command, 1 if not handled. */
int ts3plugin_processCommand(uint64 serverConnectionHandlerID, const char* command) {
	return 1;  /* Plugin didn't handle command */
}

/* Client changed current server connection handler */
void ts3plugin_currentServerConnectionChanged(uint64 serverConnectionHandlerID) {
}

/*
 * Implement the following three functions when the plugin should display a line in the server/channel/client info.
 * If any of ts3plugin_infoTitle, ts3plugin_infoData or ts3plugin_freeMemory is missing, the info text will not be displayed.
 */

/* Required to release the memory for parameter "data" allocated in ts3plugin_infoData and ts3plugin_initMenus */
void ts3plugin_freeMemory(void* data) {
	free(data);
}

/*
 * Plugin requests to be always automatically loaded by the TeamSpeak 3 client unless
 * the user manually disabled it in the plugin dialog.
 * This function is optional. If missing, no autoload is assumed.
 */
int ts3plugin_requestAutoload() {
	return 1;  /* 1 = request autoloaded, 0 = do not request autoload */
}

/************************** TeamSpeak callbacks ***************************/
/*
 * Following functions are optional, feel free to remove unused callbacks.
 * See the clientlib documentation for details on each function.
 */

/* Clientlib */

void ts3plugin_onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber)
{
	if (newStatus == STATUS_CONNECTED)
	{
		// get server name
		char* serverId;
		if (ts3Functions.getServerVariableAsString(serverConnectionHandlerID, VIRTUALSERVER_UNIQUE_IDENTIFIER, &serverId) == ERROR_ok)
		{
			char *serverName;
			if (ts3Functions.getServerVariableAsString(serverConnectionHandlerID, VIRTUALSERVER_NAME, &serverName) == ERROR_ok)
			{
				QSettings settings(WAYFSettingsFile, QSettings::IniFormat);
				QStringList lst = settings.childGroups();
				if (!lst.contains(serverId))
				{
					settings.beginGroup(serverId);
					settings.setValue("ServerName", serverName);
					settings.setValue("ChannelGroupAdmin", 0);
					settings.setValue("ChannelGroupFriends", 0);
					settings.setValue("ChannelGroupBlocked", 0);
					settings.setValue("AutoKick", false);
					settings.endGroup();
					settings.sync();
				}
			}
			ts3Functions.freeMemory(serverId);
		}
	}
}

uint64 WAYFGetChannelGroupAdmin(char* serverId)
{
	uint64 ret = 0;
	QSettings settings(WAYFSettingsFile, QSettings::IniFormat);
	QString strServerId = QString::fromUtf8(serverId);
	ret = settings.value(strServerId + "/ChannelGroupAdmin", 0).toLongLong();
	return ret;
}

uint64 WAYFGetChannelGroupFriend(uint64 serverConnectionHandlerId)
{
	char* serverId;
	if (ts3Functions.getServerVariableAsString(serverConnectionHandlerId, VIRTUALSERVER_UNIQUE_IDENTIFIER, &serverId) == ERROR_ok)
	{
		QSettings settings(WAYFSettingsFile, QSettings::IniFormat);
		QString strServerId = QString::fromUtf8(serverId);
		return settings.value(strServerId + "/ChannelGroupFriends", 0).toLongLong();
	}
	ts3Functions.freeMemory(serverId);
	return 0;
}

uint64 WAYFGetChannelGroupBlocked(uint64 serverConnectionHandlerId)
{
	char* serverId;
	if (ts3Functions.getServerVariableAsString(serverConnectionHandlerId, VIRTUALSERVER_UNIQUE_IDENTIFIER, &serverId) == ERROR_ok)
	{
		QSettings settings(WAYFSettingsFile, QSettings::IniFormat);
		QString strServerId = QString::fromUtf8(serverId);
		return settings.value(strServerId + "/ChannelGroupBlocked", 0).toLongLong();
	}
	ts3Functions.freeMemory(serverId);
	return 0;
}

uint64 CurrentServerConnectionHandlerID;
anyID CurrentClientID;
static int SQL_onClientMoveEventCallback(void *unused, int count, char **data, char **columns)
{
	if (count == 3)
	{
		QString strData = QString::fromUtf8(data[2]);
		QStringList lstAllData = strData.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
		if (lstAllData.count() > 1)
		{
			if (lstAllData.at(1).indexOf("Friend=") > -1)
			{
				QString strFriendStatus = lstAllData.at(1);
				int friendStatus = strFriendStatus.replace(QString("Friend="), QString()).toInt();
				if (friendStatus == 0 || friendStatus == 1)
				{
					anyID ownClientID;
					if (ts3Functions.getClientID(CurrentServerConnectionHandlerID, &ownClientID) == ERROR_ok)
					{
						uint64 ownChannelID;
						if (ts3Functions.getChannelOfClient(CurrentServerConnectionHandlerID, ownClientID, &ownChannelID) == ERROR_ok)
						{
							uint64 clientDatabaseID;
							uint64 clientDatabaseIDArray[1];
							uint64 targetChannel[1] = { ownChannelID };
							if (CurrentServerConnectionHandlerID != NULL && CurrentClientID != NULL)
							{
								if (ts3Functions.getClientVariableAsUInt64(CurrentServerConnectionHandlerID, CurrentClientID, CLIENT_DATABASE_ID, &clientDatabaseID) == ERROR_ok)
								{
									uint64 channelGroup[1];
									clientDatabaseIDArray[0] = clientDatabaseID;
									switch (friendStatus)
									{
									case 0:
										// client is friend
										channelGroup[0] = WAYFGetChannelGroupFriend(CurrentServerConnectionHandlerID);
										ts3Functions.requestSetClientChannelGroup(CurrentServerConnectionHandlerID, channelGroup, targetChannel, clientDatabaseIDArray, 1, NULL);
										break;
									case 1:
										// client is blocked
										channelGroup[0] = WAYFGetChannelGroupBlocked(CurrentServerConnectionHandlerID);
										ts3Functions.requestSetClientChannelGroup(CurrentServerConnectionHandlerID, channelGroup, targetChannel, clientDatabaseIDArray, 1, NULL);
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

void onClientMoveEvent_thread(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage)
{
	// get server id
	char* serverId;
	if (ts3Functions.getServerVariableAsString(serverConnectionHandlerID, VIRTUALSERVER_UNIQUE_IDENTIFIER, &serverId) == ERROR_ok)
	{
		QSettings settings(WAYFSettingsFile, QSettings::IniFormat);
		QStringList lst = settings.childGroups();
		if (lst.contains(serverId))
		{
			anyID ownClientId;
			char sql[255];
			if (ts3Functions.getClientID(serverConnectionHandlerID, &ownClientId) == ERROR_ok)
			{
				if (clientID != ownClientId)
				{
					uint64 ownChannelId;
					if (ts3Functions.getChannelOfClient(serverConnectionHandlerID, ownClientId, &ownChannelId) == ERROR_ok)
					{
						if (newChannelID == ownChannelId)
						{
							uint64 ownChannelGroupId;
							if (ts3Functions.getClientVariableAsUInt64(serverConnectionHandlerID, ownClientId, CLIENT_CHANNEL_GROUP_ID, &ownChannelGroupId) == ERROR_ok)
							{
								if (ownChannelGroupId == WAYFGetChannelGroupAdmin(serverId))
								{
									char *clientUniqueIdentifier;
									if (ts3Functions.getClientVariableAsString(serverConnectionHandlerID, clientID, CLIENT_UNIQUE_IDENTIFIER, &clientUniqueIdentifier) == ERROR_ok)
									{
										int rc;
										CurrentServerConnectionHandlerID = serverConnectionHandlerID;
										CurrentClientID = clientID;

										strcpy(sql, "SELECT * from Contacts WHERE value LIKE '%IDS=");
										strcat(sql, clientUniqueIdentifier);
										strcat(sql, "'");
										rc = sqlite3_exec(db, sql, SQL_onClientMoveEventCallback, NULL, NULL);
										if (rc != SQLITE_OK)
										{
											printf("WAYF: SQL error.\n");
										}

										ts3Functions.freeMemory(clientUniqueIdentifier);

										CurrentServerConnectionHandlerID = NULL;
										CurrentClientID = NULL;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	ts3Functions.freeMemory(serverId);
}

void ts3plugin_onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage) {
	std::thread thr(onClientMoveEvent_thread, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, moveMessage);
	thr.detach();
}