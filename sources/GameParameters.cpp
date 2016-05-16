#ifdef _WIN32
#include "windows.h"
#include "glut.h"
#endif

#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include <time.h>

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_net.h"

#include "F1Spirit.h"
#include "sound.h"
#include "List.h"
#include "2DCMC.h"
#include "auxiliar.h"
#include "GLTile.h"
#include "PlacedGLTile.h"
#include "RotatedGLTile.h"
#include "keyboardstate.h"
#include "CPlayer.h"
#include "CCar.h"
#include "2DCMC.h"
#include "RoadPiece.h"
#include "track.h"
#include "RacingCCar.h"
#include "PlayerCCar.h"
#include "EnemyCCar.h"
#include "GameParameters.h"
#include "ReplayInfo.h"
#include "F1SpiritGame.h"
#include "F1SpiritApp.h"
#include "F1Spirit-auxiliar.h"
#include "randomc.h"
#include "filehandling.h"

#include "debug.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif


F1S_GParameters::F1S_GParameters(void)
{
	int rc[N_TRACKS] = {54, 23, 43, 43, 18, 26, 28, 28, 30, 30, 32, 32, 34, 34, 36, 36, 38, 38, 40, 40, 42,
	                    80, 80, 16, 16
	                   };
	float hc[N_TRACKS] = {0.95F, 1.0F, 1.0F, 1.0F, 1.05F,
	                      0.975F, 0.977F, 0.979F, 0.981F,
	                      0.983F, 0.985F, 0.987F, 0.989F,
	                      0.991F, 0.993F, 0.9F, 0.995F,
	                      0.997F, 1.0F, 1.05F, 1.1F,

	                      1.0F, 1.0F, 1.0F, 1.0F
	                     };
	float hd[N_TRACKS] = {0.003, 0.002, 0.002, 0.002, 0.005,
	                      0.004, 0.004, 0.004, 0.004,
	                      0.004, 0.004, 0.004, 0.004,
	                      0.004, 0.004, 0.004, 0.004,
	                      0.004, 0.004, 0.004, 0.004,

	                      0.002, 0.002, 0.005, 0.005
	                     };

	float rp[N_TRACKS] = {0.1, 0.5, 0.25, 0.25, 0.1,
	                      0.1, 0.15, 0.2, 0.1,
	                      0.1, 0.25, 0.8, 0.4,
	                      0.15, 0.2, 0.1, 0.1,
	                      0.1, 0.0, 0.2, 0.5,

	                      0.0, 0.0, 0.0, 0.0
	                     };

	float PC[6] = {0.0580F, 0.0580F, 0.0575F, 0.0570F, 0.0564F, 0.0558F};
	float adh[6] = {0.7F, 1.0F, 1.0F, 1.0F, 0.85F, 1.0F};
	int st[6] = {200, 500, 200, 200, 200, 200};
	int i;

	/* Game parameters: */

	for (i = 0;i < N_TRACKS;i++) {
		race_cars[i] = rc[i];
		base_handycap[i] = hc[i];
		handycap_decrement[i] = hd[i];
		rain_probability[i] = rp[i];
	} 

	for (i = 0;i < 6;i++) {
		PIXEL_CTNTS[i] = PC[i];
		car_type_adherence[i] = adh[i];
		car_type_spinning_threshold[i] = st[i];
	} 

	ntracks = N_TRACKS;

	SPEED_CTNT = 0.00124F;

	MAX_FUEL = 1000;

	fuel_consumption_k1 = 13500;

	fuel_consumption_k2 = 7000;

	fuel_recharge_speed = 1.66F;

	repair_speed = 0.001F;

	road_adherence = 0.8F;

	grass_adherence = 0.55F;

	sand_adherence = 0.25F;

	road_friction = 0.0004F;

	grass_friction = 0.0015F;

	sand_friction = 0.00055F;

	squeal_threshold = 10;

	spinning_threshold = 10;

	rtyre_damage_influence = 0.33F;

	ftyre_damage_influence = 0.25F;

	engine_damage_influence = 0.125F;

	brake_damage_influence = 0.5F;

} 


bool F1S_GParameters::load_ascii(char *fname)
{
	FILE *fp;
	bool error = false;

	/* Parameters load: */
	fp = f1open(fname, "r", GAMEDATA);

	if (fp != 0) {
		int i;
		char token[256];

#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("Parameter configuration file found, reading parameters...\n");
#endif

		if (read_parameters_token(fp, token))
			sscanf(token, "%i", &ntracks);

		for (i = 0;i < ntracks;i++) {
			if (read_parameters_token(fp, token))
				sscanf(token, "%i", &(race_cars[i]));
		} 

		for (i = 0;i < ntracks;i++) {
			if (read_parameters_token(fp, token))
				sscanf(token, "%f", &(base_handycap[i]));

			if (read_parameters_token(fp, token))
				sscanf(token, "%f", &(handycap_decrement[i]));
		} 

		for (i = 0;i < ntracks;i++) {
			if (read_parameters_token(fp, token))
				sscanf(token, "%f", &(rain_probability[i]));
		} 

		for (i = 0;i < 6;i++) {
			if (read_parameters_token(fp, token))
				sscanf(token, "%f", &(PIXEL_CTNTS[i]));
		} 

		if (read_parameters_token(fp, token))
			sscanf(token, "%f", &SPEED_CTNT);

		if (read_parameters_token(fp, token))
			sscanf(token, "%f", &MAX_FUEL);

		if (read_parameters_token(fp, token))
			sscanf(token, "%f", &fuel_consumption_k1);

		if (read_parameters_token(fp, token))
			sscanf(token, "%f", &fuel_consumption_k2);

		if (read_parameters_token(fp, token))
			sscanf(token, "%f", &fuel_recharge_speed);

		if (read_parameters_token(fp, token))
			sscanf(token, "%f", &repair_speed);

		if (read_parameters_token(fp, token))
			sscanf(token, "%f", &road_adherence);

		if (read_parameters_token(fp, token))
			sscanf(token, "%f", &grass_adherence);

		if (read_parameters_token(fp, token))
			sscanf(token, "%f", &sand_adherence);

		if (read_parameters_token(fp, token))
			sscanf(token, "%f", &road_friction);

		if (read_parameters_token(fp, token))
			sscanf(token, "%f", &grass_friction);

		if (read_parameters_token(fp, token))
			sscanf(token, "%f", &sand_friction);

		for (i = 0;i < 6;i++) {
			if (read_parameters_token(fp, token))
				sscanf(token, "%f", &(car_type_adherence[i]));
		} 

		for (i = 0;i < 6;i++) {
			if (read_parameters_token(fp, token))
				sscanf(token, "%i", &(car_type_spinning_threshold[i]));
		} 

		if (read_parameters_token(fp, token))
			sscanf(token, "%i", &squeal_threshold);

		if (read_parameters_token(fp, token))
			sscanf(token, "%i", &spinning_threshold);

		if (read_parameters_token(fp, token))
			sscanf(token, "%f", &rtyre_damage_influence);

		if (read_parameters_token(fp, token))
			sscanf(token, "%f", &ftyre_damage_influence);

		if (read_parameters_token(fp, token))
			sscanf(token, "%f", &engine_damage_influence);

		if (read_parameters_token(fp, token))
			sscanf(token, "%f", &brake_damage_influence);

#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("Parameter configuration load finished.\n");

#endif

		fclose(fp);
	} 

	return error;
} 


bool F1S_GParameters::read_parameters_token(FILE *fp, char *token)
{
	int i = 0;
	int c;

	c = fgetc(fp);

	do {
		while (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			c = fgetc(fp);

		if (c == ';') {
			c = fgetc(fp);

			while (c != '\n' && !feof(fp))
				c = fgetc(fp);

			if (c == '\r')
				c = fgetc(fp);
		} 

		if (feof(fp))
			return false;
	} while (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == ';');

	while (c != 0 && c != ' ' && c != '\t' && c != '\r' && c != '\n' && !feof(fp)) {
		token[i++] = c;
		c = fgetc(fp);
	} 

	token[i] = 0;

	if (i > 0)
		return true;

	return false;
} 



bool F1S_GParameters::load_bin(FILE *fp)
{
	int i;

	ntracks = fgetc(fp);

	for (i = 0;i < ntracks;i++) {
		race_cars[i] = fgetc(fp);
		race_cars[i] += fgetc(fp) * 256;
	} 

	for (i = 0;i < ntracks;i++) {
		load_float(fp, &(base_handycap[i]));
		load_float(fp, &(handycap_decrement[i]));
	} 

	for (i = 0;i < ntracks;i++) {
		load_float(fp, &(rain_probability[i]));
	} 

	for (i = 0;i < 6;i++) {
		load_float(fp, &(PIXEL_CTNTS[i]));
	} 

	load_float(fp, &SPEED_CTNT);

	load_float(fp, &MAX_FUEL);

	load_float(fp, &fuel_consumption_k1);

	load_float(fp, &fuel_consumption_k2);

	load_float(fp, &fuel_recharge_speed);

	load_float(fp, &repair_speed);

	load_float(fp, &road_adherence);

	load_float(fp, &grass_adherence);

	load_float(fp, &sand_adherence);

	load_float(fp, &road_friction);

	load_float(fp, &grass_friction);

	load_float(fp, &sand_friction);

	for (i = 0;i < 6;i++) {
		load_float(fp, &(car_type_adherence[i]));
	} 

	for (i = 0;i < 6;i++) {
		car_type_spinning_threshold[i] = fgetc(fp);
		car_type_spinning_threshold[i] += fgetc(fp) * 256;
	} 

	squeal_threshold = fgetc(fp);

	squeal_threshold += fgetc(fp) * 256;

	spinning_threshold = fgetc(fp);

	spinning_threshold += fgetc(fp) * 256;

	load_float(fp, &rtyre_damage_influence);

	load_float(fp, &ftyre_damage_influence);

	load_float(fp, &engine_damage_influence);

	load_float(fp, &brake_damage_influence);

	return true;
} 



bool F1S_GParameters::load_bin_AC8(FILE *fp)
{
	int i;

	ntracks = fgetc(fp);

	for (i = 0;i < ntracks;i++) {
		race_cars[i] = fgetc(fp);
		race_cars[i] += fgetc(fp) * 256;
	} 

	for (i = 0;i < ntracks;i++) {
		load_float(fp, &(base_handycap[i]));
		load_float(fp, &(handycap_decrement[i]));
	} 

	for (i = 0;i < 6;i++) {
		load_float(fp, &(PIXEL_CTNTS[i]));
	} 

	load_float(fp, &SPEED_CTNT);

	load_float(fp, &MAX_FUEL);

	load_float(fp, &fuel_consumption_k1);

	load_float(fp, &fuel_consumption_k2);

	load_float(fp, &fuel_recharge_speed);

	load_float(fp, &repair_speed);

	load_float(fp, &road_adherence);

	load_float(fp, &grass_adherence);

	load_float(fp, &sand_adherence);

	load_float(fp, &road_friction);

	load_float(fp, &grass_friction);

	load_float(fp, &sand_friction);

	for (i = 0;i < 6;i++) {
		load_float(fp, &(car_type_adherence[i]));
	} 

	for (i = 0;i < 6;i++) {
		car_type_spinning_threshold[i] = fgetc(fp);
		car_type_spinning_threshold[i] += fgetc(fp) * 256;
	} 

	squeal_threshold = fgetc(fp);

	squeal_threshold += fgetc(fp) * 256;

	spinning_threshold = fgetc(fp);

	spinning_threshold += fgetc(fp) * 256;

	load_float(fp, &rtyre_damage_influence);

	load_float(fp, &ftyre_damage_influence);

	load_float(fp, &engine_damage_influence);

	load_float(fp, &brake_damage_influence);

	return true;
} 


bool F1S_GParameters::load_bin_AC5(FILE *fp)
{
	int i;

	ntracks = 21;

	for (i = 0;i < ntracks;i++) {
		race_cars[i] = fgetc(fp);
		race_cars[i] += fgetc(fp) * 256;
	} 

	for (i = 0;i < ntracks;i++) {
		load_float(fp, &(base_handycap[i]));
		load_float(fp, &(handycap_decrement[i]));
	} 

	for (i = 0;i < 6;i++) {
		load_float(fp, &(PIXEL_CTNTS[i]));
	} 

	load_float(fp, &SPEED_CTNT);

	load_float(fp, &MAX_FUEL);

	load_float(fp, &fuel_consumption_k1);

	load_float(fp, &fuel_consumption_k2);

	load_float(fp, &fuel_recharge_speed);

	load_float(fp, &repair_speed);

	load_float(fp, &road_adherence);

	load_float(fp, &grass_adherence);

	load_float(fp, &sand_adherence);

	load_float(fp, &road_friction);

	load_float(fp, &grass_friction);

	load_float(fp, &sand_friction);

	for (i = 0;i < 6;i++) {
		load_float(fp, &(car_type_adherence[i]));
	} 

	for (i = 0;i < 6;i++) {
		car_type_spinning_threshold[i] = fgetc(fp);
		car_type_spinning_threshold[i] += fgetc(fp) * 256;
	} 

	squeal_threshold = fgetc(fp);

	squeal_threshold += fgetc(fp) * 256;

	spinning_threshold = fgetc(fp);

	spinning_threshold += fgetc(fp) * 256;

	load_float(fp, &rtyre_damage_influence);

	load_float(fp, &ftyre_damage_influence);

	load_float(fp, &engine_damage_influence);

	load_float(fp, &brake_damage_influence);

	return true;
} 


bool F1S_GParameters::save_bin(FILE *fp)
{
	int i;

	fputc(ntracks, fp);

	for (i = 0;i < ntracks;i++) {
		fputc(race_cars[i]&0x0ff, fp);
		fputc(race_cars[i] >> 8, fp);
	} 

	for (i = 0;i < ntracks;i++) {
		save_float(fp, base_handycap[i]);
		save_float(fp, handycap_decrement[i]);
	} 

	for (i = 0;i < ntracks;i++) {
		save_float(fp, rain_probability[i]);
	} 

	for (i = 0;i < 6;i++) {
		save_float(fp, PIXEL_CTNTS[i]);
	} 

	save_float(fp, SPEED_CTNT);

	save_float(fp, MAX_FUEL);

	save_float(fp, fuel_consumption_k1);

	save_float(fp, fuel_consumption_k2);

	save_float(fp, fuel_recharge_speed);

	save_float(fp, repair_speed);

	save_float(fp, road_adherence);

	save_float(fp, grass_adherence);

	save_float(fp, sand_adherence);

	save_float(fp, road_friction);

	save_float(fp, grass_friction);

	save_float(fp, sand_friction);

	for (i = 0;i < 6;i++) {
		save_float(fp, car_type_adherence[i]);
	} 

	for (i = 0;i < 6;i++) {
		fputc(car_type_spinning_threshold[i]&0x0ff, fp);
		fputc(car_type_spinning_threshold[i] >> 8, fp);
	} 

	fputc(squeal_threshold&0x0ff, fp);

	fputc(squeal_threshold >> 8, fp);

	fputc(spinning_threshold&0x0ff, fp);

	fputc(spinning_threshold >> 8, fp);

	save_float(fp, rtyre_damage_influence);

	save_float(fp, ftyre_damage_influence);

	save_float(fp, engine_damage_influence);

	save_float(fp, brake_damage_influence);

	return true;
} 


void F1S_GParameters::instance(F1S_GParameters *p)
{
	int i;

	/* Game parameters: */
	ntracks = p->ntracks;

	for (i = 0;i < ntracks;i++) {
		race_cars[i] = p->race_cars[i];
		base_handycap[i] = p->base_handycap[i];
		handycap_decrement[i] = p->handycap_decrement[i];
		rain_probability[i] = p->rain_probability[i];
	} 

	for (i = 0;i < 6;i++) {
		PIXEL_CTNTS[i] = p->PIXEL_CTNTS[i];
		car_type_adherence[i] = p->car_type_adherence[i];
		car_type_spinning_threshold[i] = p->car_type_spinning_threshold[i];
	} 

	SPEED_CTNT = p->SPEED_CTNT;

	MAX_FUEL = p->MAX_FUEL;

	fuel_consumption_k1 = p->fuel_consumption_k1;

	fuel_consumption_k2 = p->fuel_consumption_k2;

	fuel_recharge_speed = p->fuel_recharge_speed;

	repair_speed = p->repair_speed;

	road_adherence = p->road_adherence;

	grass_adherence = p->grass_adherence;

	sand_adherence = p->sand_adherence;

	road_friction = p->road_friction;

	grass_friction = p->grass_friction;

	sand_friction = p->sand_friction;

	squeal_threshold = p->squeal_threshold;

	spinning_threshold = p->spinning_threshold;

	rtyre_damage_influence = p->rtyre_damage_influence;

	ftyre_damage_influence = p->ftyre_damage_influence;

	engine_damage_influence = p->engine_damage_influence;

	brake_damage_influence = p->brake_damage_influence;
} 

