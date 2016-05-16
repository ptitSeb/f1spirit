#ifndef _F1SPIRIT_TRACKVIEWER
#define _F1SPIRIT_TRACKVIEWER




class F1SpiritTrackViewer
{

		friend class F1SpiritApp;

	public:
		F1SpiritTrackViewer(int ntrack);
		~F1SpiritTrackViewer();

		void initialize_track(int ntrack);

		bool cycle(void);
		void draw(void);

		int get_nlaps(void);

	private:

		CTrack *track;
		List<GLTile> tiles;

		float c_z, c_a, c_x, c_y, vdx, vdy;

		GLTile *mini_tile;

		/* RACE: */
		SDL_Surface *race_minimap_car_sfc, *race_minimap_enemycar_sfc;
		GLTile *race_minitrack;
		float race_minimap_zoom;
		List<CRoadPiece> road_position;
		int nlaps;
		float speed;

		GLTile *road_tile[3];
		GLTile *road_ltile[2];
		GLTile *road_rtile[2];
		GLTile *road_lines;
};

#endif
