#ifndef _F1SPIRIT_RACINGCCAR
#define _F1SPIRIT_RACINGCCAR



class RacingCCar
{

	public:
		RacingCCar();
		~RacingCCar();

		virtual bool save(FILE *fp);

		virtual bool load_status(FILE *fp);
		virtual bool save_status(FILE *fp);

		void pauseSFX() {
			car->pauseSFX();
		}

		void resumeSFX() {
			car->resumeSFX();
		}

		CCar *car;
		CTrack *track;
		List<CRoadPiece> road_position;
		int laps;
		bool first_lap;

		float position;
};


#endif
