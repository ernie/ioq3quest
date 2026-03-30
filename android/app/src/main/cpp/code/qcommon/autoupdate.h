#ifndef __AUTOUPDATE_H__
#define __AUTOUPDATE_H__

#ifdef USE_HTTP

typedef enum {
	UPDATE_IDLE,        // no update activity
	UPDATE_CHECKING,    // querying GitHub API
	UPDATE_AVAILABLE,   // newer version found, waiting for user
	UPDATE_DOWNLOADING, // downloading APK
	UPDATE_READY,       // APK downloaded, ready to install
	UPDATE_ERROR        // something went wrong
} updateState_t;

void Update_Init( void );
void Update_Frame( void );
void Update_Shutdown( void );

// console commands
void Update_Check_f( void );
void Update_Download_f( void );
void Update_Cancel_f( void );
void Update_Install_f( void );

#else

// stubs when HTTP is disabled
#define Update_Init()
#define Update_Frame()
#define Update_Shutdown()

#endif // USE_HTTP

#endif // __AUTOUPDATE_H__
