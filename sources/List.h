#ifndef GENERIC_LIST
#define GENERIC_LIST

/*

  Author: Santi Ontañón Villar
  Last Modified: 4-11-2005

*/

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

/*
Funciones para LISTAS:

 void Delete()
 void Instance(List<T> &l)
 void GoTo(int i);
 void Rewind(void)
 void Forward(void)
 void Next(void)
 void Prev(void)
 T *GetObj(void)
 void SetObj(T *o)
 void SetObj(int i,T *o)
 LLink<T> *GetPos(void)
 void SetPos(LLink<T> *)
 bool EmptyP()
 bool EndP()
 bool LastP()
 bool BeginP()
 void Insert(T *o)
 void Add(T *o)
 void AddAfter(LLink<T> *pos,T *o)
 void AddBefore(LLink<T> *pos,T *o)
 bool Iterate(T *&o)
 T *ExtractIni(void)
 T *Extract(void)
 bool MemberP(T *o)
 T *MemberGet(T *o)
 bool MemberRefP(T *o)
 int Position(T *o);
 int PositionRef(T *o);
 int Length()
 void Copy(List<T> &l)    // not available in the <char> lists
 void CopyRefs(List<T> &l)    // not available in the <char> lists
 void Synchronize(List<T> *l);
 void Append(List<T> &l)
 bool DeleteElement(T *o)
 bool DeletePosition(int i);
 T *GetRandom(void)
 int SearchObjRef(T *o)
 int SearchObj(T *o)
 void SetNoOriginal(void)
 void SetOriginal(void)
 void Sort(bool *p(T *o1,T *o2))

 bool integrity_test(void);

 bool operator==(List<T> &l)
 T *operator[](int index)
*/

template <class T>

class LLink
{

	public:
		LLink<T>(T *o, LLink<T> *n = 0) {
			obj = o;
			next = n;
		};

		~LLink<T>() {
			delete obj;

			if (next != 0)
				delete next;
		};

		inline LLink<T> *Getnext() {
			return next;
		};

		inline void Setnext(LLink<T> *n) {
			next = n;
		};

		inline T *GetObj() {
			return obj;
		};

		inline void SetObj(T *o) {
			obj = o;
		};

		void Anade(T *o) {
			if (next == 0) {
				LLink<T> *node = new LLink<T>(o);
				next = node;
			} else {
				next->Anade(o);
			}
		};

	private:
		T *obj;
		LLink<T> *next;
};

template <class T>

class List
{

	public:
		List<T>() {
			list = 0;
			act = 0;
			top = 0;
			original = true;
		};

		~List<T>() {
			if (original) {
				T *o;

				while (!EmptyP()) {
					o = ExtractIni();
					delete o;
				} /* while */

				delete list;
			} /* if */
		};

		List<T>(List<T> &l) {
			list = 0;
			act = 0;
			top = 0;
			original = true;
			Copy(l);
		};

		void Delete() {
			if (original) {
				T *o;

				while (!EmptyP()) {
					o = ExtractIni();
					delete o;
				} /* while */
			} /* if */

			list = 0;

			act = 0;

			top = 0;
		};

		void Instance(List<T> &l) {
			list = l.list;
			act = list;
			top = l.top;
			original = false;
		};

		void Rewind(void) {
			act = list;
		};

		void Forward(void) {
			act = top;
		};

		void Next(void) {
			if (act != 0)
				act = act->Getnext();
		};

		void Prev(void) {
			LLink<T> *tmp;

			if (act != list) {
				tmp = list;

				while (tmp->Getnext() != act)
					tmp = tmp->Getnext();

				act = tmp;
			} /* if */
		};


		void GoTo(int index) {
			act = list;

			while (act != 0 && index > 0) {
				act = act->Getnext();
				index--;
			} /* while */
		};

		T *GetObj(void) {
			return act->GetObj();
		};

		void SetObj(T *o) {
			act->SetObj(o);
		};

		void SetObj(int i, T *o) {
			act->SetObj(o);
		};


		LLink<T> *GetPos(void) {
			return act;
		};

		void SetPos(LLink<T> *p) {
			act = p;
		};

		bool EmptyP() {
			return list == 0;
		};

		bool EndP() {
			return act == 0;
		};

		bool LastP() {
			return act == top;
		};

		bool BeginP() {
			return act == list;
		};

		void Insert(T *o) {
			if (list == 0) {
				list = new LLink<T>(o);
				top = list;
			} else {
				list = new LLink<T>(o, list);
			} /* if */

			if (act == 0)
				act = list;
		};

		void Add(T *o) {
			if (list == 0) {
				list = new LLink<T>(o);
				top = list;
			} else {
				top->Anade(o);
				top = top->Getnext();
			} /* if */

			if (act == 0)
				act = list;
		};

		void AddAfter(LLink<T> *pos, T *o) {
			if (pos == 0) {
				if (list == 0) {
					list = new LLink<T>(o);
					top = list;
				} else {
					list = new LLink<T>(o, list);
				} /* if */
			} else {
				LLink<T> *nl = new LLink<T>(o);

				nl->Setnext(pos->Getnext());
				pos->Setnext(nl);

				if (nl->Getnext() == 0)
					top = nl;
			} /* if */

			if (act == 0)
				act = list;
		} /* AddAfter */

		void AddBefore(LLink<T> *pos, T *o) {
			if (pos == list) {
				if (list == 0) {
					list = new LLink<T>(o);
					top = list;
				} else {
					list = new LLink<T>(o, list);
				} /* if */
			} else {
				LLink<T> *l, *nl = new LLink<T>(o);

				l = list;

				while (l->Getnext() != pos)
					l = l->Getnext();

				l->Setnext(nl);

				nl->Setnext(pos);

				if (pos == 0)
					top = nl;
			} /* if */

			if (act == 0)
				act = list;
		} /* AddBefore */

		T *operator[](int index) {
			LLink<T> *tmp = list;

			while (tmp != 0 && index > 0) {
				tmp = tmp->Getnext();
				index--;
			} /* while */

			if (tmp == 0)
				return 0;

			return tmp->GetObj();
		};

		bool Iterate(T *&o) {
			if (EndP())
				return false;

			o = act->GetObj();

			act = act->Getnext();

			return true;
		} /* Iterate */

		T *ExtractIni(void) {
			LLink<T> *tmp;
			T *o;

			if (list == 0)
				return 0;

			o = list->GetObj();

			tmp = list;

			list = list->Getnext();

			tmp->Setnext(0);

			if (act == tmp)
				act = list;

			if (list == 0)
				top = 0;

			tmp->SetObj(0);

			delete tmp;

			return o;
		} /* ExtractIni */

		T *Extract(void) {
			LLink<T> *tmp, *tmp2 = 0;
			T *o;

			if (list == 0)
				return 0;

			tmp = list;

			while (tmp->Getnext() != 0) {
				tmp2 = tmp;
				tmp = tmp->Getnext();
			} /* while */

			o = tmp->GetObj();

			if (tmp2 == 0) {
				list = 0;
				top = 0;
				act = 0;
			} else {
				tmp2->Setnext(0);
				top = tmp2;
			} /* if */

			if (act == tmp)
				act = top;

			tmp->SetObj(0);

			delete tmp;

			return o;
		} /* Extract */

		bool MemberP(T *o) {
			LLink<T> *tmp;
			tmp = list;

			while (tmp != 0) {
				if (*(tmp->GetObj()) == *o)
					return true;

				tmp = tmp->Getnext();
			} /* while */

			return false;
		} /* MemberP */

		T *MemberGet(T *o) {
			LLink<T> *tmp;
			tmp = list;

			while (tmp != 0) {
				if (*(tmp->GetObj()) == *o)
					return tmp->GetObj();

				tmp = tmp->Getnext();
			} /* while */

			return 0;
		} /* MemberGet */

		bool MemberRefP(T *o) {
			LLink<T> *tmp;
			tmp = list;

			while (tmp != 0) {
				if (tmp->GetObj() == o)
					return true;

				tmp = tmp->Getnext();
			} /* while */

			return false;
		} /* MemberRefP */

		int Position(T *o) {
			LLink<T> *tmp;
			int i = 0;
			tmp = list;

			while (tmp != 0) {
				if (*(tmp->GetObj()) == *o)
					return i;

				i++;

				tmp = tmp->Getnext();
			} /* while */

			return -1;
		} /* Position */

		int PositionRef(T *o) {
			LLink<T> *tmp;
			int i = 0;
			tmp = list;

			while (tmp != 0) {
				if (tmp->GetObj() == o)
					return i;

				i++;

				tmp = tmp->Getnext();
			} /* while */

			return -1;
		} /* PositionRef */

		int Length() {
			LLink<T> *tmp;
			int count = 0;

			tmp = list;

			while (tmp != 0) {
				tmp = tmp->Getnext();
				count++;
			} /* while */

			return count;
		};

		void Copy(List<T> &l) {
			List<T> ltmp;
			T *o;
			Delete();
			original = true;

			ltmp.Instance(l);
			ltmp.Rewind();

			while (ltmp.Iterate(o)) {
				o = new T(*o);
				Add(o);
			} /* while */

			Synchronize(&l);
		} /* Copy */

		void CopyRefs(List<T> &l) {
			List<T> ltmp;
			T *o;
			Delete();
			original = true;

			ltmp.Instance(l);
			ltmp.Rewind();

			while (ltmp.Iterate(o)) {
				Add(o);
			} /* while */

			Synchronize(&l);
		} /* CopyRefs */

		void Synchronize(List<T> *l) {
			LLink<T> *ll;

			ll = l->list;
			act = list;

			while (ll != 0 && ll != l->act) {
				ll = ll->Getnext();

				if (act != 0)
					act = act->Getnext();
			} /* while */
		} /* Synchronize */


		void Append(List<T> &l) {
			T *o;

			l.Rewind();

			while (l.Iterate(o)) {
				o = new T(*o);
				Add(o);
			} /* while */
		} /* Append */


		bool DeleteElement(T *o) {
			LLink<T> *tmp1, *tmp2;

			tmp1 = list;
			tmp2 = 0;

			while (tmp1 != 0 && tmp1->GetObj() != o) {
				tmp2 = tmp1;
				tmp1 = tmp1->Getnext();
			} /* while */

			if (tmp1 != 0) {
				if (tmp2 == 0) {
					/* Eliminar el primer elemento de la lista: */
					list = list->Getnext();
					tmp1->Setnext(0);

					if (act == tmp1)
						act = list;

					tmp1->SetObj(0);

					delete tmp1;

					if (list == 0)
						top = 0;
				} else {
					/* Eliminar un elemento intermedio: */
					tmp2->Setnext(tmp1->Getnext());

					if (act == tmp1)
						act = tmp1->Getnext();

					if (top == tmp1)
						top = tmp2;

					tmp1->Setnext(0);

					tmp1->SetObj(0);

					delete tmp1;
				} /* if */

				return true;
			} else {
				return false;
			} /* if */

		} /* DeleteElement */


		bool DeletePosition(int i) {
			LLink<T> *tmp1, *tmp2;

			tmp1 = list;
			tmp2 = 0;

			while (tmp1 != 0 && i > 0) {
				i--;
				tmp2 = tmp1;
				tmp1 = tmp1->Getnext();
			} /* while */

			if (tmp1 != 0) {
				if (tmp2 == 0) {
					/* Eliminar el primer elemento de la lista: */
					list = list->Getnext();
					tmp1->Setnext(0);

					if (act == tmp1)
						act = list;

					tmp1->SetObj(0);

					delete tmp1;
				} else {
					/* Eliminar un elemento intermedio: */
					tmp2->Setnext(tmp1->Getnext());

					if (act == tmp1)
						act = tmp1->Getnext();

					if (top == tmp1)
						top = tmp2;

					tmp1->Setnext(0);

					tmp1->SetObj(0);

					delete tmp1;
				} /* if */

				return true;
			} else {
				return false;
			} /* if */
		} /* DeletePosition */



		T *GetRandom(void) {
			int i, l = Length();
			i = ((rand() * l) / RAND_MAX);

			if (i == l)
				i = l - 1;

			return operator[](i);
		} /* GetRandom */

		bool operator==(List<T> &l) {
			LLink<T> *tmp1, *tmp2;

			tmp1 = list;
			tmp2 = l.list;

			while (tmp1 != 0 && tmp2 != 0) {
				if (!((*(tmp1->GetObj())) == (*(tmp2->GetObj()))))
					return false;

				tmp1 = tmp1->Getnext();

				tmp2 = tmp2->Getnext();
			} /* while */

			return tmp1 == tmp2;
		} /* == */


		int SearchObjRef(T *o) {
			LLink<T> *tmp;
			int pos = 0;

			tmp = list;

			while (tmp != 0) {
				if ((tmp->GetObj()) == o)
					return pos;

				tmp = tmp->Getnext();

				pos++;
			} /* while */

			return -1;
		} /* SearchObj */

		int SearchObj(T *o) {
			LLink<T> *tmp;
			int pos = 0;

			tmp = list;

			while (tmp != 0) {
				if (*(tmp->GetObj()) == *o)
					return pos;

				tmp = tmp->Getnext();

				pos++;
			} /* while */

			return -1;
		} /* SearchObj */


		void Sort(bool (*p)(T *o1, T *o2)) {
			LLink<T> *l1, *l2;
			T* tmp;
			bool change;

			do {
				change = false;
				l1 = 0;
				l2 = list;

				while (l2 != 0) {
					if (l1 != 0 && l2 != 0) {
						if (!(*p)(l1->GetObj(), l2->GetObj())) {
							tmp = l1->GetObj();
							l1->SetObj(l2->GetObj());
							l2->SetObj(tmp);
							change = true;
						} /* if */
					} /* if */

					l1 = l2;

					l2 = l2->Getnext();
				} /* while */
			} while (change);
		} /* Sort */

		bool integrity_test(void) {
			List< LLink<T> > l;
			LLink<T> *pos;

			pos = list;

			while (pos != 0) {
				if (l.MemberRefP(pos))
					return false;

				l.Add(pos);

				pos = pos->Getnext();
			} /* while */

			if (!l.MemberRefP(top))
				return false;

			while (!l.EmptyP())
				l.ExtractIni();

			if (list != 0) {
				if (top == 0)
					return false;

				if (top->Getnext() != 0)
					return false;
			} else {
				if (top != 0)
					return false;
			} /* if */

			return true;
		} /* integrity_test */


		void SetNoOriginal(void) {
			original = false;
		}

		void SetOriginal(void) {
			original = true;
		}

	private:
		bool original;
		LLink<T> *list, *top;
		LLink<T> *act;
};


/* CHAR SPECIAL CASE: */


template <>

class LLink<char>
{

	public:
		LLink(char *o, LLink<char> *n = 0) {
			obj = o;
			next = n;
		};

		~LLink() {
			delete []obj;

			if (next != 0)
				delete next;
		};

		inline LLink *Getnext() {
			return next;
		};

		inline void Setnext(LLink *n) {
			next = n;
		};

		inline char *GetObj() {
			return obj;
		};

		inline void SetObj(char *o) {
			obj = o;
		};

		void Anade(char *o) {
			if (next == 0) {
				LLink *node = new LLink(o);
				next = node;
			} else {
				next->Anade(o);
			}
		};

	private:
		char *obj;
		LLink *next;
};


template <>

class List<char>
{

	public:
		List() {
			list = 0;
			act = 0;
			top = 0;
			original = true;
		};

		~List() {
			if (original) {
				char *o;

				while (!EmptyP()) {
					o = ExtractIni();
					delete []o;
				} /* while */

				delete list;
			} /* if */
		};

		void Delete() {
			if (original) {
				char *o;

				while (!EmptyP()) {
					o = ExtractIni();
					delete []o;
				} /* while */
			} /* if */

			list = 0;

			act = 0;

			top = 0;
		};

		void Instance(List<char> &l) {
			list = l.list;
			act = list;
			top = l.top;
			original = false;
		};

		void Rewind(void) {
			act = list;
		};

		void Forward(void) {
			act = top;
		};

		void Next(void) {
			if (act != 0)
				act = act->Getnext();
		};

		void Prev(void) {
			LLink<char> *tmp;

			if (act != list) {
				tmp = list;

				while (tmp->Getnext() != act)
					tmp = tmp->Getnext();

				act = tmp;
			} /* if */
		};

		void GoTo(int index) {
			act = list;

			while (act != 0 && index > 0) {
				act = act->Getnext();
				index--;
			} /* while */
		};

		char *GetObj(void) {
			return act->GetObj();
		};

		void SetObj(char *o) {
			act->SetObj(o);
		};

		void SetObj(int i, char *o) {
			act->SetObj(o);
		};

		LLink<char> *GetPos(void) {
			return act;
		};

		void SetPos(LLink<char> *p) {
			act = p;
		};

		bool EmptyP() {
			return list == 0;
		};

		bool EndP() {
			return act == 0;
		};

		bool LastP() {
			return act == top;
		};

		bool BeginP() {
			return act == list;
		};

		void Insert(char *o) {
			if (list == 0) {
				list = new LLink<char>(o);
				top = list;
			} else {
				list = new LLink<char>(o, list);
			} /* if */

			if (act == 0)
				act = list;
		};

		void Add(char *o) {
			if (list == 0) {
				list = new LLink<char>(o);
				top = list;
			} else {
				top->Anade(o);
				top = top->Getnext();
			} /* if */

			if (act == 0)
				act = list;
		};

		void AddAfter(LLink<char> *pos, char *o) {
			if (pos == 0) {
				if (list == 0) {
					list = new LLink<char>(o);
					top = list;
				} else {
					list = new LLink<char>(o, list);
				} /* if */
			} else {
				LLink<char> *nl = new LLink<char>(o);

				nl->Setnext(pos->Getnext());
				pos->Setnext(nl);

				if (nl->Getnext() == 0)
					top = nl;
			} /* if */

			if (act == 0)
				act = list;
		} /* AddAfter */

		void AddBefore(LLink<char> *pos, char *o) {
			if (pos == list) {
				if (list == 0) {
					list = new LLink<char>(o);
					top = list;
				} else {
					list = new LLink<char>(o, list);
				} /* if */
			} else {
				LLink<char> *l, *nl = new LLink<char>(o);

				l = list;

				while (l->Getnext() != pos)
					l = l->Getnext();

				l->Setnext(nl);

				nl->Setnext(pos);

				if (pos == 0)
					top = nl;
			} /* if */

			if (act == 0)
				act = list;
		} /* AddBefore */

		char *operator[](int index) {
			LLink<char> *tmp = list;

			while (tmp != 0 && index > 0) {
				tmp = tmp->Getnext();
				index--;
			} /* while */

			if (tmp == 0)
				throw;

			return tmp->GetObj();
		};

		bool Iterate(char *&o) {
			if (EndP())
				return false;

			o = act->GetObj();

			act = act->Getnext();

			return true;
		} /* Iterate */

		char *ExtractIni(void) {
			LLink<char> *tmp;
			char *o;

			if (list == 0)
				return 0;

			o = list->GetObj();

			tmp = list;

			list = list->Getnext();

			tmp->Setnext(0);

			if (act == tmp)
				act = list;

			if (top == act)
				top = 0;

			tmp->SetObj(0);

			delete tmp;

			return o;
		} /* ExtractIni */

		char *Extract(void) {
			LLink<char> *tmp, *tmp2 = 0;
			char *o;

			if (list == 0)
				return 0;

			tmp = list;

			while (tmp->Getnext() != 0) {
				tmp2 = tmp;
				tmp = tmp->Getnext();
			} /* while */

			o = tmp->GetObj();

			if (tmp2 == 0) {
				list = 0;
				top = 0;
				act = 0;
			} else {
				tmp2->Setnext(0);
				top = tmp2;
			} /* if */

			if (act == tmp)
				act = top;

			tmp->SetObj(0);

			delete tmp;

			return o;
		} /* Extract */

		bool MemberP(char *o) {
			LLink<char> *tmp;
			tmp = list;

			while (tmp != 0) {
				if (*(tmp->GetObj()) == *o)
					return true;

				tmp = tmp->Getnext();
			} /* while */

			return false;
		} /* MemberP */

		char *MemberGet(char *o) {
			LLink<char> *tmp;
			tmp = list;

			while (tmp != 0) {
				if (*(tmp->GetObj()) == *o)
					return tmp->GetObj();

				tmp = tmp->Getnext();
			} /* while */

			return 0;
		} /* MemberGet */

		bool MemberRefP(char *o) {
			LLink<char> *tmp;
			tmp = list;

			while (tmp != 0) {
				if (tmp->GetObj() == o)
					return true;

				tmp = tmp->Getnext();
			} /* while */

			return false;
		} /* MemberRefP */

		int Position(char *o) {
			LLink<char> *tmp;
			int i = 0;
			tmp = list;

			while (tmp != 0) {
				if (strcmp(tmp->GetObj(), o) == 0)
					return i;

				i++;

				tmp = tmp->Getnext();
			} /* while */

			return -1;
		} /* Position */

		int PositionRef(char *o) {
			LLink<char> *tmp;
			int i = 0;
			tmp = list;

			while (tmp != 0) {
				if (tmp->GetObj() == o)
					return i;

				i++;

				tmp = tmp->Getnext();
			} /* while */

			return -1;
		} /* PositionRef */

		int Length() {
			LLink<char> *tmp;
			int count = 0;

			tmp = list;

			while (tmp != 0) {
				tmp = tmp->Getnext();
				count++;
			} /* while */

			return count;
		};

		void Synchronize(List<char> *l) {
			LLink<char> *ll;

			ll = l->list;
			act = list;

			while (ll != 0 && ll != l->act) {
				ll = ll->Getnext();

				if (act != 0)
					act = act->Getnext();
			} /* while */
		} /* Synchronize */

		bool DeleteElement(char *o) {
			LLink<char> *tmp1, *tmp2;

			tmp1 = list;
			tmp2 = 0;

			while (tmp1 != 0 && tmp1->GetObj() != o) {
				tmp2 = tmp1;
				tmp1 = tmp1->Getnext();
			} /* while */

			if (tmp1 != 0) {
				if (tmp2 == 0) {
					/* Eliminar el primer elemento de la lista: */
					list = list->Getnext();
					tmp1->Setnext(0);

					if (act == tmp1)
						act = list;

					tmp1->SetObj(0);

					delete tmp1;
				} else {
					/* Eliminar un elemento intermedio: */
					tmp2->Setnext(tmp1->Getnext());

					if (act == tmp1)
						act = tmp1->Getnext();

					if (top == tmp1)
						top = tmp2;

					tmp1->Setnext(0);

					tmp1->SetObj(0);

					delete tmp1;
				} /* if */

				return true;
			} else {
				return false;
			} /* if */

		} /* DeleteElement */

		bool DeletePosition(int i) {
			LLink<char> *tmp1, *tmp2;

			tmp1 = list;
			tmp2 = 0;

			while (tmp1 != 0 && i > 0) {
				i--;
				tmp2 = tmp1;
				tmp1 = tmp1->Getnext();
			} /* while */

			if (tmp1 != 0) {
				if (tmp2 == 0) {
					/* Eliminar el primer elemento de la lista: */
					list = list->Getnext();
					tmp1->Setnext(0);

					if (act == tmp1)
						act = list;

					tmp1->SetObj(0);

					delete tmp1;
				} else {
					/* Eliminar un elemento intermedio: */
					tmp2->Setnext(tmp1->Getnext());

					if (act == tmp1)
						act = tmp1->Getnext();

					if (top == tmp1)
						top = tmp2;

					tmp1->Setnext(0);

					tmp1->SetObj(0);

					delete tmp1;
				} /* if */

				return true;
			} else {
				return false;
			} /* if */
		} /* DeletePosition */


		char *GetRandom(void) {
			int i, l = Length();
			i = ((rand() * l) / RAND_MAX);

			if (i == l)
				i = l - 1;

			return operator[](i);
		} /* GetRandom */

		int SearchObjRef(char *o) {
			LLink<char> *tmp;
			int pos = 0;

			tmp = list;

			while (tmp != 0) {
				if ((tmp->GetObj()) == o)
					return pos;

				tmp = tmp->Getnext();

				pos++;
			} /* while */

			return -1;
		} /* SearchObj */

		int SearchObj(char *o) {
			LLink<char> *tmp;
			int pos = 0;

			tmp = list;

			while (tmp != 0) {
				if (*(tmp->GetObj()) == *o)
					return pos;

				tmp = tmp->Getnext();

				pos++;
			} /* while */

			return -1;
		} /* SearchObj */


		void Sort(bool (*p)(char *o1, char *o2)) {
			LLink<char> *l1, *l2;
			char* tmp;
			bool change;

			do {
				change = false;
				l1 = 0;
				l2 = list;

				while (l2 != 0) {
					if (l1 != 0 && l2 != 0) {
						if (!(*p)(l1->GetObj(), l2->GetObj())) {
							tmp = l1->GetObj();
							l1->SetObj(l2->GetObj());
							l2->SetObj(tmp);
							change = true;
						} /* if */
					} /* if */

					l1 = l2;

					l2 = l2->Getnext();
				} /* while */
			} while (change);
		} /* Sort */

		bool integrity_test(void) {
			List< LLink<char> > l;
			LLink<char> *pos;

			pos = list;

			while (pos != 0) {
				if (l.MemberRefP(pos))
					return false;

				l.Add(pos);

				pos = pos->Getnext();
			} /* while */

			if (!l.MemberRefP(top))
				return false;

			while (!l.EmptyP())
				l.ExtractIni();

			if (list != 0) {
				if (top == 0)
					return false;

				if (top->Getnext() != 0)
					return false;
			} else {
				if (top != 0)
					return false;
			} /* if */

			return true;
		} /* integrity_test */

		void SetNoOriginal(void) {
			original = false;
		}

		void SetOriginal(void) {
			original = true;
		}

	private:
		bool original;
		LLink<char> *list, *top;
		LLink<char> *act;
};


#endif
