#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL.h>

#include "List.h"
#include "F1Spirit.h"
#include "CPlayer.h"
#include "filehandling.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

extern char *f1spirit_replay_version;


CPlayerInfo::CPlayerInfo(void)
{
	int i;

	name = new char[strlen("PRACTICE MODE") + 1];
	strcpy(name, "PRACTICE MODE");

	encrypted_password = new char[strlen("NONE") + 1];
	strcpy(encrypted_password, "NONE");

	for (i = 0;i < N_TRACKS;i++)
		race_points[i] = 0;

	for (i = 0;i < N_TRACKS;i++)
		race_time[i] = 0;

	for (i = 0;i < N_TRACKS;i++)
		race_bestlap[i] = 0;

	for (i = 0;i < N_TRACKS;i++)
		race_position[i] = -1;

	for (i = 0;i < N_TRACKS;i++) {
		time_date[i].year = 0;
		time_date[i].month = 0;
		time_date[i].day = 0;
		lap_date[i].year = 0;
		lap_date[i].month = 0;
		lap_date[i].day = 0;
		position_date[i].year = 0;
		position_date[i].month = 0;
		position_date[i].day = 0;
	} 

	/* keyboard configuration: */
	nplayers = 1;

	up = new Uint32[1];

	down = new Uint32[1];

	left = new Uint32[1];

	right = new Uint32[1];

	accelerate = new Uint32[1];

	brake = new Uint32[1];

	joystick = new int[1];

	zoom_speed = new int[1];

	up[0] = SDLK_UP;

	down[0] = SDLK_DOWN;

	left[0] = SDLK_LEFT;

	right[0] = SDLK_RIGHT;

	accelerate[0] = SDLK_SPACE;

	brake[0] = SDLK_m;

	joystick[0] = -1;

	zoom_speed[0] = 4;

	add_player();

	add_player();

	add_player();

	up[1] = SDLK_w;

	down[1] = SDLK_s;

	left[1] = SDLK_a;

	right[1] = SDLK_d;

	accelerate[1] = SDLK_LSHIFT;

	brake[1] = SDLK_LCTRL;

	joystick[0] = -1;

	music_volume = 128;

	sfx_volume = 64;

} 


CPlayerInfo::CPlayerInfo(char *n, char *password)
{
	int i;
	char tmp[128];

	encrypt_password(password, tmp);

	name = new char[strlen(n) + 1];
	strcpy(name, n);

	encrypted_password = new char[strlen(tmp) + 1];
	strcpy(encrypted_password, tmp);

	for (i = 0;i < N_TRACKS;i++)
		race_points[i] = 0;

	for (i = 0;i < N_TRACKS;i++)
		race_time[i] = 0;

	for (i = 0;i < N_TRACKS;i++)
		race_bestlap[i] = 0;

	for (i = 0;i < N_TRACKS;i++)
		race_position[i] = -1;

	for (i = 0;i < N_TRACKS;i++) {
		time_date[i].year = 0;
		time_date[i].month = 0;
		time_date[i].day = 0;
		lap_date[i].year = 0;
		lap_date[i].month = 0;
		lap_date[i].day = 0;
		position_date[i].year = 0;
		position_date[i].month = 0;
		position_date[i].day = 0;
	} 

	/* keyboard configuration: */
	nplayers = 1;

	up = new Uint32[1];

	down = new Uint32[1];

	left = new Uint32[1];

	right = new Uint32[1];

	accelerate = new Uint32[1];

	brake = new Uint32[1];

	joystick = new int[1];

	zoom_speed = new int[1];

	up[0] = SDLK_UP;

	down[0] = SDLK_DOWN;

	left[0] = SDLK_LEFT;

	right[0] = SDLK_RIGHT;

	accelerate[0] = SDLK_SPACE;

	brake[0] = SDLK_m;

	joystick[0] = -1;

	zoom_speed[0] = 4;

	add_player();

	add_player();

	add_player();

	up[1] = SDLK_w;

	down[1] = SDLK_s;

	left[1] = SDLK_a;

	right[1] = SDLK_d;

	accelerate[1] = SDLK_LSHIFT;

	brake[1] = SDLK_LCTRL;

	joystick[0] = -1;

	music_volume = 128;

	sfx_volume = 64;

} 


CPlayerInfo::~CPlayerInfo()
{
	delete []name;
	name = 0;

	delete []encrypted_password;
	encrypted_password = 0;

	delete []up;
	up = 0;
	delete []down;
	down = 0;
	delete []left;
	left = 0;
	delete []right;
	right = 0;
	delete []accelerate;
	accelerate = 0;
	delete []brake;
	brake = 0;
	delete []joystick;
	joystick = 0;
	delete []zoom_speed;
	zoom_speed = 0;
} 


void CPlayerInfo::set_points(int i, int p)
{
#ifdef _WIN32

	struct tm today;

	_tzset();
	_getsystime(&today);

	position_date[i].year = today.tm_year;
	position_date[i].month = today.tm_mon;
	position_date[i].day = today.tm_mday;
#else

	struct timeval ttime;

	struct tm *today;

	gettimeofday(&ttime, NULL);
	today = localtime(&(ttime.tv_sec));

	position_date[i].year = today->tm_year;
	position_date[i].month = today->tm_mon;
	position_date[i].day = today->tm_mday;
#endif

	race_points[i] = p;
} 


void CPlayerInfo::set_time(int i, Uint32 p)
{
#ifdef _WIN32

	struct tm today;

	_tzset();
	_getsystime(&today);

	time_date[i].year = today.tm_year;
	time_date[i].month = today.tm_mon;
	time_date[i].day = today.tm_mday;
#else

	struct timeval ttime;

	struct tm *today;

	gettimeofday(&ttime, NULL);
	today = localtime(&(ttime.tv_sec));

	time_date[i].year = today->tm_year;
	time_date[i].month = today->tm_mon;
	time_date[i].day = today->tm_mday;
#endif

	race_time[i] = p;
} 


void CPlayerInfo::set_bestlap(int i, Uint32 p)
{
#ifdef _WIN32

	struct tm today;

	_tzset();
	_getsystime(&today);

	lap_date[i].year = today.tm_year;
	lap_date[i].month = today.tm_mon;
	lap_date[i].day = today.tm_mday;
#else

	struct timeval ttime;

	struct tm *today;

	gettimeofday(&ttime, NULL);
	today = localtime(&(ttime.tv_sec));

	lap_date[i].year = today->tm_year;
	lap_date[i].month = today->tm_mon;
	lap_date[i].day = today->tm_mday;
#endif

	race_bestlap[i] = p;
} 


void CPlayerInfo::set_position(int i, int p)
{
#ifdef _WIN32

	struct tm today;

	_tzset();
	_getsystime(&today);

	position_date[i].year = today.tm_year;
	position_date[i].month = today.tm_mon;
	position_date[i].day = today.tm_mday;
#else

	struct timeval ttime;

	struct tm *today;

	gettimeofday(&ttime, NULL);
	today = localtime(&(ttime.tv_sec));

	position_date[i].year = today->tm_year;
	position_date[i].month = today->tm_mon;
	position_date[i].day = today->tm_mday;
#endif

	race_position[i] = p;
} 



bool CPlayerInfo::load(FILE *fp, int ntracks, char *version)
{
	unsigned int i;
	unsigned int nl;
	Uint32 v;
	char *p;

	/* player name: */
	nl = fgetc(fp);
	delete []name;
	name = 0;

	if (nl < 1 || nl > 16) {
		fclose(fp);
		return false;
	} 

	name = new char[nl + 1];

	for (i = 0;i < nl;i++)
		name[i] = fgetc(fp);

	name[nl] = 0;

	/* password: */
	nl = fgetc(fp);

	delete []encrypted_password;

	encrypted_password = 0;

	if (nl < 1 || nl > 32) {
		fclose(fp);
		return false;
	} 

	encrypted_password = new char[nl + 1];

	for (i = 0;i < nl;i++)
		encrypted_password[i] = fgetc(fp);

	encrypted_password[nl] = 0;

	for (i = 0;i < (unsigned)ntracks;i++)
		race_points[i] = fgetc(fp);

	for (i = 0;i < (unsigned)ntracks;i++) {
		p = (char *) & v;
		p[0] = fgetc(fp);
		p[1] = fgetc(fp);
		p[2] = fgetc(fp);
		p[3] = fgetc(fp);
		race_time[i] = v;
	} 

	for (i = 0;i < (unsigned)ntracks;i++)
		race_position[i] = fgetc(fp);

	for (i = 0;i < (unsigned)ntracks;i++) {
		p = (char *) & v;
		p[0] = fgetc(fp);
		p[1] = fgetc(fp);
		p[2] = fgetc(fp);
		p[3] = fgetc(fp);
		race_bestlap[i] = v;
	} 

	for (i = 0;i < (unsigned)ntracks;i++) {
		time_date[i].year = fgetc(fp);
		time_date[i].month = fgetc(fp);
		time_date[i].day = fgetc(fp);
		lap_date[i].year = fgetc(fp);
		lap_date[i].month = fgetc(fp);
		lap_date[i].day = fgetc(fp);
		position_date[i].year = fgetc(fp);
		position_date[i].month = fgetc(fp);
		position_date[i].day = fgetc(fp);
	} 

	nplayers = fgetc(fp);

	delete []up;

	up = new Uint32[nplayers];

	delete []down;

	down = new Uint32[nplayers];

	delete []left;

	left = new Uint32[nplayers];

	delete []right;

	right = new Uint32[nplayers];

	delete []accelerate;

	accelerate = new Uint32[nplayers];

	delete []brake;

	brake = new Uint32[nplayers];

	delete []zoom_speed;

	zoom_speed = new int[nplayers];

	for (i = 0;i < nplayers;i++) {
		p = (char *) & v;
		p[0] = fgetc(fp);
		p[1] = fgetc(fp);
		p[2] = fgetc(fp);
		p[3] = fgetc(fp);
		up[i] = v;
		p[0] = fgetc(fp);
		p[1] = fgetc(fp);
		p[2] = fgetc(fp);
		p[3] = fgetc(fp);
		down[i] = v;
		p[0] = fgetc(fp);
		p[1] = fgetc(fp);
		p[2] = fgetc(fp);
		p[3] = fgetc(fp);
		left[i] = v;
		p[0] = fgetc(fp);
		p[1] = fgetc(fp);
		p[2] = fgetc(fp);
		p[3] = fgetc(fp);
		right[i] = v;
		p[0] = fgetc(fp);
		p[1] = fgetc(fp);
		p[2] = fgetc(fp);
		p[3] = fgetc(fp);
		accelerate[i] = v;
		p[0] = fgetc(fp);
		p[1] = fgetc(fp);
		p[2] = fgetc(fp);
		p[3] = fgetc(fp);
		brake[i] = v;
		zoom_speed[i] = fgetc(fp);
	} 

	for (i = 0;i < nplayers;i++) {
		joystick[i] = fgetc(fp) - 1;
	} 

	if (strcmp(version, "F1SAC6") == 0 ||
	        strcmp(version, "F1SAC7") == 0) {
		for (i = 0;i < nplayers;i++) {
			/* default_camera[i] = */
			fgetc(fp);
		} 
	} // if

	music_volume = fgetc(fp);

	sfx_volume = fgetc(fp);

	if (strcmp(version, "F1SAC6") == 0 ||
	        strcmp(version, "F1SAC7") == 0) {
		/*scoreboard_type= */
		fgetc(fp);
	} // if

	return true;
} 



bool CPlayerInfo::load_ac5(FILE *fp)
{
	unsigned int i;
	unsigned int nl;
	Uint32 v;
	char *p;

	/* player name: */
	nl = fgetc(fp);
	delete []name;
	name = 0;

	if (nl < 1 || nl > 16) {
		fclose(fp);
		return false;
	} 

	name = new char[nl + 1];

	for (i = 0;i < nl;i++)
		name[i] = fgetc(fp);

	name[nl] = 0;

	for (i = 0;i < N_TRACKS;i++)
		race_points[i] = fgetc(fp);

	for (i = 0;i < N_TRACKS;i++) {
		p = (char *) & v;
		p[0] = fgetc(fp);
		p[1] = fgetc(fp);
		p[2] = fgetc(fp);
		p[3] = fgetc(fp);
		race_time[i] = v;
	} 

	nplayers = fgetc(fp);

	delete []up;

	up = new Uint32[nplayers];

	delete []down;

	down = new Uint32[nplayers];

	delete []left;

	left = new Uint32[nplayers];

	delete []right;

	right = new Uint32[nplayers];

	delete []accelerate;

	accelerate = new Uint32[nplayers];

	delete []brake;

	brake = new Uint32[nplayers];

	delete []zoom_speed;

	zoom_speed = new int[nplayers];

	for (i = 0;i < nplayers;i++) {
		p = (char *) & v;
		p[0] = fgetc(fp);
		p[1] = fgetc(fp);
		p[2] = fgetc(fp);
		p[3] = fgetc(fp);
		up[i] = v;
		p[0] = fgetc(fp);
		p[1] = fgetc(fp);
		p[2] = fgetc(fp);
		p[3] = fgetc(fp);
		down[i] = v;
		p[0] = fgetc(fp);
		p[1] = fgetc(fp);
		p[2] = fgetc(fp);
		p[3] = fgetc(fp);
		left[i] = v;
		p[0] = fgetc(fp);
		p[1] = fgetc(fp);
		p[2] = fgetc(fp);
		p[3] = fgetc(fp);
		right[i] = v;
		p[0] = fgetc(fp);
		p[1] = fgetc(fp);
		p[2] = fgetc(fp);
		p[3] = fgetc(fp);
		accelerate[i] = v;
		p[0] = fgetc(fp);
		p[1] = fgetc(fp);
		p[2] = fgetc(fp);
		p[3] = fgetc(fp);
		brake[i] = v;
		zoom_speed[i] = fgetc(fp);
	} 

	for (i = 0;i < nplayers;i++) {
		joystick[i] = fgetc(fp) - 1;
	} 

	for (i = 0;i < nplayers;i++) {
		/* default_camera[i] = */
		fgetc(fp);
	} 

	music_volume = fgetc(fp);

	sfx_volume = fgetc(fp);

	return true;
} 




bool CPlayerInfo::save(FILE *fp)
{
	unsigned int l, i, j;
	char *tmp, *tmp2;

	l = 1 + strlen(name) +
	    1 + strlen(encrypted_password) +
	    N_TRACKS + N_TRACKS * 4 + N_TRACKS + N_TRACKS * 4 + N_TRACKS * 9 +
	    1 + nplayers * 25 + nplayers + 2;

	tmp = new char[l];

	i = 0;
	tmp[i++] = strlen(name);

	for (j = 0;j < strlen(name);j++)
		tmp[i++] = name[j];

	tmp[i++] = strlen(encrypted_password);

	for (j = 0;j < strlen(encrypted_password);j++)
		tmp[i++] = encrypted_password[j];

	for (j = 0;j < N_TRACKS;j++)
		tmp[i++] = race_points[j];

	for (j = 0;j < N_TRACKS;j++) {
		tmp2 = (char *)(&(race_time[j]));
		tmp[i++] = tmp2[0];
		tmp[i++] = tmp2[1];
		tmp[i++] = tmp2[2];
		tmp[i++] = tmp2[3];
	} 

	for (j = 0;j < N_TRACKS;j++)
		tmp[i++] = race_position[j];

	for (j = 0;j < N_TRACKS;j++) {
		tmp2 = (char *)(&(race_bestlap[j]));
		tmp[i++] = tmp2[0];
		tmp[i++] = tmp2[1];
		tmp[i++] = tmp2[2];
		tmp[i++] = tmp2[3];
	} 

	for (j = 0;j < N_TRACKS;j++) {
		tmp[i++] = time_date[j].year;
		tmp[i++] = time_date[j].month;
		tmp[i++] = time_date[j].day;
		tmp[i++] = lap_date[j].year;
		tmp[i++] = lap_date[j].month;
		tmp[i++] = lap_date[j].day;
		tmp[i++] = position_date[j].year;
		tmp[i++] = position_date[j].month;
		tmp[i++] = position_date[j].day;
	} 

	tmp[i++] = nplayers;

	for (j = 0;j < nplayers;j++) {
		tmp2 = (char *)(&(up[j]));
		tmp[i++] = tmp2[0];
		tmp[i++] = tmp2[1];
		tmp[i++] = tmp2[2];
		tmp[i++] = tmp2[3];
		tmp2 = (char *)(&(down[j]));
		tmp[i++] = tmp2[0];
		tmp[i++] = tmp2[1];
		tmp[i++] = tmp2[2];
		tmp[i++] = tmp2[3];
		tmp2 = (char *)(&(left[j]));
		tmp[i++] = tmp2[0];
		tmp[i++] = tmp2[1];
		tmp[i++] = tmp2[2];
		tmp[i++] = tmp2[3];
		tmp2 = (char *)(&(right[j]));
		tmp[i++] = tmp2[0];
		tmp[i++] = tmp2[1];
		tmp[i++] = tmp2[2];
		tmp[i++] = tmp2[3];
		tmp2 = (char *)(&(accelerate[j]));
		tmp[i++] = tmp2[0];
		tmp[i++] = tmp2[1];
		tmp[i++] = tmp2[2];
		tmp[i++] = tmp2[3];
		tmp2 = (char *)(&(brake[j]));
		tmp[i++] = tmp2[0];
		tmp[i++] = tmp2[1];
		tmp[i++] = tmp2[2];
		tmp[i++] = tmp2[3];
		tmp[i++] = zoom_speed[j];
	} 

	for (j = 0;j < nplayers;j++) {
		tmp[i++] = joystick[j] + 1;
	} 

	tmp[i++] = music_volume;

	tmp[i++] = sfx_volume;

	if (l != fwrite(tmp, 1, l, fp)) return false;

	return true;
} 


void CPlayerInfo::add_player()
{
	Uint32 *nup, *ndown, *nleft, *nright, *naccelerate, *nbrake;
	int *njoystick, *ndc, *nzs;
	unsigned int i;

	nup = new Uint32[nplayers + 1];
	ndown = new Uint32[nplayers + 1];
	nleft = new Uint32[nplayers + 1];
	nright = new Uint32[nplayers + 1];
	naccelerate = new Uint32[nplayers + 1];
	nbrake = new Uint32[nplayers + 1];
	njoystick = new int[nplayers + 1];
	ndc = new int[nplayers + 1];
	nzs = new int[nplayers + 1];

	for (i = 0;i < nplayers;i++) {
		nup[i] = up[i];
		ndown[i] = down[i];
		nleft[i] = left[i];
		nright[i] = right[i];
		naccelerate[i] = accelerate[i];
		nbrake[i] = brake[i];
		njoystick[i] = joystick[i];
		nzs[i] = zoom_speed[i];
	} 

	nup[nplayers] = SDLK_UNKNOWN;

	ndown[nplayers] = SDLK_UNKNOWN;

	nleft[nplayers] = SDLK_UNKNOWN;

	nright[nplayers] = SDLK_UNKNOWN;

	naccelerate[nplayers] = SDLK_UNKNOWN;

	nbrake[nplayers] = SDLK_UNKNOWN;

	njoystick[nplayers] = -1;

	ndc[nplayers] = 2;

	nzs[nplayers] = 2;

	delete []up;

	delete []down;

	delete []left;

	delete []right;

	delete []accelerate;

	delete []brake;

	delete []joystick;

	delete []zoom_speed;

	up = nup;

	down = ndown;

	left = nleft;

	right = nright;

	accelerate = naccelerate;

	brake = nbrake;

	joystick = njoystick;

	zoom_speed = nzs;

	nplayers++;
} 


void CPlayerInfo::sub_player()
{
	Uint32 *nup, *ndown, *nleft, *nright, *naccelerate, *nbrake;
	int *njoystick, *ndc, *nzs;
	unsigned int i;

	if (nplayers > 1) {
		nup = new Uint32[nplayers - 1];
		ndown = new Uint32[nplayers - 1];
		nleft = new Uint32[nplayers - 1];
		nright = new Uint32[nplayers - 1];
		naccelerate = new Uint32[nplayers - 1];
		nbrake = new Uint32[nplayers - 1];
		njoystick = new int[nplayers - 1];
		ndc = new int[nplayers - 1];
		nzs = new int[nplayers - 1];

		for (i = 0;i < nplayers - 1;i++) {
			nup[i] = up[i];
			ndown[i] = down[i];
			nleft[i] = left[i];
			nright[i] = right[i];
			naccelerate[i] = accelerate[i];
			nbrake[i] = brake[i];
			njoystick[i] = joystick[i];
			nzs[i] = zoom_speed[i];
		} 

		delete []up;

		delete []down;

		delete []left;

		delete []right;

		delete []accelerate;

		delete []brake;

		delete []joystick;

		delete []zoom_speed;

		up = nup;

		down = ndown;

		left = nleft;

		right = nright;

		accelerate = naccelerate;

		brake = nbrake;

		joystick = njoystick;

		zoom_speed = nzs;

		nplayers--;
	} 
} 


int CPlayerInfo::get_points(void)
{
	int i;
	int points = 0;

	for (i = 0;i < 21;i++)
		points += race_points[i];

	return points;
} 


int get_player_list(List<char> *names)
{
	FILE *fp;
	int i;
	char filename[256], *tmp;
	bool found;

	i = 0;

	do {
		found = false;
		sprintf(filename, "players/player%i.dat", i);
		fp = f1open(filename, "rb", USERDATA);

		if (fp != 0) {
			CPlayerInfo *pi = new CPlayerInfo();
			pi->load(fp, N_TRACKS, f1spirit_replay_version);
			tmp = new char[strlen(pi->get_name()) + 1];
			strcpy(tmp, pi->get_name());
			names->Add(tmp);
			delete pi;
			found = true;
			fclose(fp);
		} 

		i++;
	} while (found);

	return names->Length();
} /* get_player_names */



void CPlayerInfo::encrypt_password(char *password, char *result)
{
	char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	int key[16] = {6, 16, 84, 71, 129, 201, 38, 118, 247, 96, 84, 16, 169, 216, 107, 66};
	int n, n1, n2;
	int l;
	int i, j, k;

	l = strlen(password);

	j = 0;
	k = 0;

	for (i = 0;i < l;i++) {
		n = password[i];

		n = n ^ key[k];
		k++;
		k = k % 16;

		n1 = n % 0x0f;
		n2 = (n >> 4) % 0x0f;

		result[j++] = hex[n1];
		result[j++] = hex[n2];
	} 

	result[j] = 0;

} 


