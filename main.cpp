//============================================================================
// Filename:	kab342_hw13_Q1
//============================================================================

#include<iostream>
#include<cstdlib>
#include<limits>

using namespace std;

const int WIDTH(20), HEIGHT(20);

void printAnts();
void advanceSimulation();
void starveDbugs();

class Organism{
public:
	Organism() : type('-'),loc_x(0),loc_y(0),orgId(orgCnt),age(0) {orgCnt++; updateOrgList(); }
	Organism(char value, int x, int y) : type(value),loc_x(x),loc_y(y),orgId(orgCnt),age(0)
		{orgCnt++; updateOrgList(); }
	virtual ~Organism() {shrinkOrgList(this); orgCnt--; }
	static int orgCnt;
	static Organism** allOrgs;
	void updateOrgList();
	void shrinkOrgList(Organism* org);
	virtual void move() {}
	virtual void breed() {}
//  virtual void starve(); //Don't think this is necessary

	friend void advanceSimulation();

	char getType() const {return type; }
	unsigned getX() const {return loc_x; }
	unsigned getY() const {return loc_y; }
	void setX(int x) { loc_x = x; }
	void setY(int y) { loc_y = y; }
	unsigned getAge() const {return age; }
	void incrAge() {age++; }
//	virtual int getTotCnt() const {return orgCnt; }
private:
	unsigned age;
	unsigned orgId;
	char type;
	unsigned loc_x, loc_y;
};

class Ant : public Organism{
public:
	Ant() : Organism('o', 0, 0),timeTilSpawn(3),antId(antCnt)
		{antCnt++; updateAntList(); }
	Ant(int x, int y) : Organism('o', x, y),timeTilSpawn(3),antId(antCnt)
		{antCnt++; updateAntList(); }
	~Ant() {shrinkAntList(this); antCnt--; }
	static int antCnt;
	static Ant** allAnts;
	void updateAntList();
	void shrinkAntList(Ant* ant);
	void move();
	void breed();
//	void starve();
	void spawnCycle() {timeTilSpawn--; }
	unsigned getTimeTilSpawn() const {return timeTilSpawn; }
	void resetTimeTilSpawn() {timeTilSpawn = 3; }
	unsigned getId() const {return antId; }
//	int getTotCnt() const {return antCnt; }
	friend void printAnts();
private:
	unsigned antId;
	unsigned timeTilSpawn;
};

class Dbug : public Organism{
public:
	Dbug() : Organism('X', 0, 0),timeTilSpawn(8),timeTilStarve(3),dbugId(dbugCnt)
		{dbugCnt++; updateDbugList(); }
	Dbug(int x, int y) : Organism('X', x, y),timeTilSpawn(8),timeTilStarve(3),dbugId(dbugCnt)
		{dbugCnt++; updateDbugList(); }
	~Dbug() {shrinkDbugList(this); dbugCnt--; }
	static int dbugCnt;
	static Dbug** allDbugs;
	void updateDbugList();
	void shrinkDbugList(Dbug* dbug);
	void move();
	void breed();
	void didntEat() {timeTilStarve--; }
	unsigned getTimeTilStarve() const {return timeTilStarve; }
	void resetStarve() {timeTilStarve = 3; }
	void spawnCycle() {timeTilSpawn--; }
	unsigned getTimeTilSpawn() const {return timeTilSpawn; }
	void resetTimeTilSpawn() {timeTilSpawn = 8; }
	unsigned getId() const {return dbugId; }
	void setId(int value) {dbugId = value; }
//	int getTotCnt() const {return dbugCnt; }

	friend void starveDbugs();
private:
	unsigned dbugId;
	unsigned timeTilSpawn;
	unsigned timeTilStarve;
};

class Space{
public:
	Space() : occupant(NULL),loc_x(0),loc_y(0) {spaceCnt++; }
	Space(unsigned x, unsigned y) : occupant(NULL),loc_x(x),loc_y(y)
		{spaceCnt++; }
	~Space() {spaceCnt--; }
	friend void printGameboard(Space** gameboard, int width, int height);
	Organism* getOccupant() const {return occupant; }
	void setOccupant(Organism* creature);
private:
	static int spaceCnt;
	Organism* occupant;
	unsigned loc_x, loc_y;
};

int Space::spaceCnt = 0;
int Organism::orgCnt = 0;
int Ant::antCnt = 0;
int Dbug::dbugCnt = 0;
Organism** Organism::allOrgs = NULL;
Ant** Ant::allAnts = NULL;
Dbug** Dbug::allDbugs = NULL;
Space** gameboard = NULL;

int main(){

	//Initialize gameboard
	gameboard = new Space*[WIDTH*HEIGHT];
	for(int cnt1(0); cnt1 < WIDTH; cnt1++){
		for(int cnt2(0); cnt2 < HEIGHT; cnt2++){
			gameboard[(cnt1*WIDTH)+cnt2] = new Space(cnt1, cnt2);
		}
	}

	//Randomly Generate Ants
	unsigned numAnts;
	unsigned randX, randY;
	Organism* temp;
	cout<<"How many ants do you want?: ";
	cin>>numAnts;
	cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
	for(int cnt(0); cnt < numAnts; cnt++){
		randX = rand() % WIDTH;
		randY = rand() % HEIGHT;
		if((*gameboard[(randX*WIDTH)+randY]).getOccupant() == NULL){
			temp = new Ant(randX, randY);
			(*gameboard[(randX*WIDTH)+randY]).setOccupant(temp);
		} else
			cnt--;
	}

	cout<<endl;
	temp = NULL;

	//Randomly Generate Dbugs
		unsigned numDbugs;
		cout<<"How many dbugs do you want?: ";
		cin>>numDbugs;
		cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
		cout<<endl;
		for(int cnt(0); cnt < numDbugs; cnt++){
			randX = rand() % WIDTH;
			randY = rand() % HEIGHT;
			if((*gameboard[(randX*WIDTH)+randY]).getOccupant() == NULL){
				temp = new Dbug(randX, randY);
				(*gameboard[(randX*WIDTH)+randY]).setOccupant(temp);
			} else
				cnt--;
		}

	cout<<endl;
	temp = NULL;

	int turnCnt(0);

	printGameboard(gameboard, WIDTH, HEIGHT);
	cin.ignore();

	while(Organism::orgCnt != 0 && Organism::orgCnt != (WIDTH*HEIGHT)){
		advanceSimulation();
		turnCnt++;

//		//A compass and origin...
//		cout<<"   3   "<<" (0,0)(0,1)"<<endl
//			<<"   |   "<<" (1,0)(1,1)"<<endl
//			<<"2--X--0"<<endl
//			<<"   |   "<<endl
//			<<"   1   "<<endl
//			<<endl;

		cout<<"KEY:"<<endl
			<<"-----------"<<endl
			<<" (0,0)(0,1)"<<endl
			<<" (1,0)(1,1)"<<endl
			<<endl;

		cout<<"GAMEBOARD:"<<endl
			<<"----------"<<endl;

		printGameboard(gameboard, WIDTH, HEIGHT);

		cout<<endl;
		cout<<"Num organisms: "<<Organism::orgCnt<<endl
			<<"Num ants     : "<<Ant::antCnt<<endl
			<<"Num dbugs    : "<<Dbug::dbugCnt<<endl;
		cout<<endl;
		cout<<"TURN COUNT = "<<turnCnt<<endl
			<<"Press enter to continue..."<<endl<<endl;

		cin.ignore();
	}

	cout<<"GAME OVER"<<endl;

	return 1;
}

void advanceSimulation(){
	//Age all
	for(int cnt(0); cnt < Organism::orgCnt; cnt++)
		Organism::allOrgs[cnt]->incrAge();

	//Move Dbugs
	for(int cnt(0); cnt < Dbug::dbugCnt; cnt++)
		Dbug::allDbugs[cnt]->move();

	//Move Ants
	for(int cnt(0); cnt < Ant::antCnt; cnt++)
		Ant::allAnts[cnt]->move();
	cout<<endl;

	//Starve Dbugs
	starveDbugs();

	//Breed all
	for(int cnt(0); cnt < Organism::orgCnt; cnt++)
		Organism::allOrgs[cnt]->breed();

	return;
}

void printGameboard(Space** gameboard, int width, int height){
	for(int cnt1(0); cnt1 < width; cnt1++){
		cout<<" "; //Some padding out left to aid visibility...
		for(int cnt2(0); cnt2 < height; cnt2++){
			char tempType;
			if((*gameboard[(cnt1*width)+cnt2]).occupant == NULL)
				tempType = '-';
			else
				tempType = (*gameboard[(cnt1*width)+cnt2]).occupant->getType();
			cout<<tempType<<" ";
			if(cnt2 == height-1)
				cout<<endl;
		}
	}
	return;
}

void printAnts(){
	for(int cnt(0); cnt < Ant::antCnt; cnt++){
		cout<<(*Ant::allAnts[cnt]).getId()<<endl;
	}
	return;
}

void Space::setOccupant(Organism* creature){
	occupant = creature;
	return;
}

void Organism::updateOrgList(){
	if(allOrgs == NULL)
			allOrgs = new Organism*[0];
		Organism** temp = new Organism*[orgCnt];
		for(int cnt(0); cnt < orgCnt; cnt++){
			if(cnt == orgCnt-1)
				temp[cnt] = this;
			else
				temp[cnt] = allOrgs[cnt];
		}
		delete allOrgs;
		allOrgs = temp;
		return;
}

void Organism::shrinkOrgList(Organism* org){
	Organism** temp = new Organism*[orgCnt-1];
	for(int cnt(0),cnt1(0); cnt < orgCnt; cnt++){
		if (org != Organism::allOrgs[cnt]){
			temp[cnt1] = allOrgs[cnt];
			cnt1++;
		}
		else
			;
	}
	delete allOrgs;
	allOrgs = temp;
}

void Ant::updateAntList(){
	if(allAnts == NULL)
		allAnts = new Ant*[0];
	Ant** temp = new Ant*[antCnt];
	for(int cnt(0); cnt < antCnt; cnt++){
		if(cnt == antCnt-1)
			temp[cnt] = this;
		else
			temp[cnt] = allAnts[cnt];
	}
	delete allAnts;
	allAnts = temp;
	return;
}

void Ant::shrinkAntList(Ant* ant){
	Ant** temp = new Ant*[antCnt-1];
	for(int cnt(0),cnt1(0); cnt < antCnt; cnt++){
		if (ant != Ant::allAnts[cnt]){
			temp[cnt1] = allAnts[cnt];
			cnt1++;
		}
		else
			;
	}
	delete allAnts;
	allAnts = temp;
}

void Ant::move(){
	//Get random digit
	int direction = rand() % 4;
	//Check surroundings
	for(int cnt(direction); cnt < (direction+4); cnt++){
		if(cnt%4 == 0){
			if(this->getY() == HEIGHT-1)
				;
			else if((*gameboard[(this->getX()*WIDTH)+this->getY()+1]).getOccupant() == NULL){
				(*gameboard[(this->getX()*WIDTH)+this->getY()+1]).setOccupant(this);
				this->setY(this->getY()+1);
				(*gameboard[(this->getX()*WIDTH)+this->getY()-1]).setOccupant(NULL);
				break;
			}
		} else if(cnt%4 == 1){
			if(this->getX() == WIDTH-1)
				;
			else if((*gameboard[((this->getX()+1)*WIDTH)+this->getY()]).getOccupant() == NULL){
				(*gameboard[((this->getX()+1)*WIDTH)+this->getY()]).setOccupant(this);
				this->setX(this->getX()+1);
				(*gameboard[((this->getX()-1)*WIDTH)+this->getY()]).setOccupant(NULL);
				break;
			}
		} else if(cnt%4 == 2){
			if(this->getY() == 0)
				;
			else if((*gameboard[(this->getX()*WIDTH)+this->getY()-1]).getOccupant() == NULL){
				(*gameboard[(this->getX()*WIDTH)+this->getY()-1]).setOccupant(this);
				this->setY(this->getY()-1);
				(*gameboard[(this->getX()*WIDTH)+this->getY()+1]).setOccupant(NULL);
				break;
			}
		} else if(cnt%4 == 3){
			if(this->getX() == 0)
				;
			else if((*gameboard[((this->getX()-1)*WIDTH)+this->getY()]).getOccupant() == NULL){
				(*gameboard[((this->getX()-1)*WIDTH)+this->getY()]).setOccupant(this);
				this->setX(this->getX()-1);
				(*gameboard[((this->getX()+1)*WIDTH)+this->getY()]).setOccupant(NULL);
				break;
			}
		} else
			;
	}
	return;
}

void Ant::breed(){
	//Decrement timeToBreed
	if(this->getAge() < 1)
		;
	else if(this->getTimeTilSpawn() == 0)
		;
	else
		this->spawnCycle();
	//Check if needs to breed
	bool hasBred = 0;
	if(this->getTimeTilSpawn() == 0 && this->getAge() > 0){
		int direction = rand() % 4;
		for(int cnt(direction); cnt < (direction+4); cnt++){
			if(cnt%4 == 0){
				if(this->getY() == HEIGHT-1)
					;
				else if((*gameboard[(this->getX()*WIDTH)+this->getY()+1]).getOccupant() == NULL){
					Ant* temp = new Ant(this->getX(), this->getY()+1);
					hasBred = 1;
					(*gameboard[(this->getX()*WIDTH)+this->getY()+1]).setOccupant(temp);
					temp = NULL;
					break;
				}
			} else if(cnt%4 == 1){
				if(this->getX() == WIDTH-1)
					;
				else if((*gameboard[((this->getX()+1)*WIDTH)+this->getY()]).getOccupant() == NULL){
					Ant* temp = new Ant(this->getX()+1, this->getY());
					hasBred = 1;
					(*gameboard[((this->getX()+1)*WIDTH)+this->getY()]).setOccupant(temp);
					temp = NULL;
					break;
				}
			} else if(cnt%4 == 2){
				if(this->getY() == 0)
					;
				else if((*gameboard[(this->getX()*WIDTH)+this->getY()-1]).getOccupant() == NULL){
					Ant* temp = new Ant(this->getX(), this->getY()-1);
					hasBred = 1;
					(*gameboard[(this->getX()*WIDTH)+this->getY()-1]).setOccupant(temp);
					temp = NULL;
					break;
				}
			} else if(cnt%4 == 3){
				if(this->getX() == 0)
					;
				else if((*gameboard[((this->getX()-1)*WIDTH)+this->getY()]).getOccupant() == NULL){
					Ant* temp = new Ant(this->getX()-1, this->getY());
					hasBred = 1;
					(*gameboard[((this->getX()-1)*WIDTH)+this->getY()]).setOccupant(temp);
					temp = NULL;
					break;
				}
			} else
				;
		}
		if(hasBred == 1)
			this->resetTimeTilSpawn();
		else
			;
	}
	else
		;
	return;
}

void Dbug::updateDbugList(){
	if(allDbugs == NULL)
			allDbugs = new Dbug*[0];
		Dbug** temp = new Dbug*[dbugCnt];
		for(int cnt(0); cnt < dbugCnt; cnt++){
			if(cnt == dbugCnt-1)
				temp[cnt] = this;
			else
				temp[cnt] = allDbugs[cnt];
		}
		delete allDbugs;
		allDbugs = temp;
		return;
}

void Dbug::shrinkDbugList(Dbug* dbug){
	Dbug** temp = new Dbug*[dbugCnt-1];
	for(int cnt(0),cnt1(0); cnt < dbugCnt; cnt++){
		if (dbug != Dbug::allDbugs[cnt]){
			temp[cnt1] = allDbugs[cnt];
			cnt1++;
		}
		else
			;
	}
	delete allDbugs;
	allDbugs = temp;
}

void Dbug::move(){
	//Get random digit
	int direction = rand() % 4;
	//Check surroundings
	bool ateAnt(0);
	for(int cnt(direction); cnt < (direction+4); cnt++){
		if(cnt%4 == 0){
			if(this->getY() == HEIGHT-1)
				;
			else if((*gameboard[((this->getX())*WIDTH)+this->getY()+1]).getOccupant() == NULL)
				;
			else if((*gameboard[(this->getX()*WIDTH)+this->getY()+1]).getOccupant()->getType() == 'o'){
				//Delete Ant
				Ant* temp = dynamic_cast<Ant*>((*gameboard[(this->getX()*WIDTH)+this->getY()+1]).getOccupant());
				delete (*gameboard[(this->getX()*WIDTH)+this->getY()+1]).getOccupant();
				//Move Dbug
				(*gameboard[(this->getX()*WIDTH)+this->getY()+1]).setOccupant(this);
				this->setY(this->getY()+1);
				//Reset Time to Starve
				this->resetStarve();
				ateAnt = 1;
				//Vacate previously occupied space
				(*gameboard[(this->getX()*WIDTH)+this->getY()-1]).setOccupant(NULL);
				break;
			} else
				;
		} else if(cnt%4 == 1){
			if(this->getX() == WIDTH-1)
				;
			else if((*gameboard[((this->getX()+1)*WIDTH)+this->getY()]).getOccupant() == NULL)
				;
			else if((*gameboard[((this->getX()+1)*WIDTH)+this->getY()]).getOccupant()->getType() == 'o'){
				Ant* temp = dynamic_cast<Ant*>((*gameboard[((this->getX()+1)*WIDTH)+this->getY()]).getOccupant());
				delete (*gameboard[((this->getX()+1)*WIDTH)+this->getY()]).getOccupant();
				(*gameboard[((this->getX()+1)*WIDTH)+this->getY()]).setOccupant(this);
				this->setX(this->getX()+1);
				this->resetStarve();
				ateAnt = 1;
				(*gameboard[((this->getX()-1)*WIDTH)+this->getY()]).setOccupant(NULL);
				break;
			} else
				;
		} else if(cnt%4 == 2){
			if(this->getY() == 0)
				;
			else if((*gameboard[(this->getX()*WIDTH)+this->getY()-1]).getOccupant() == NULL)
				;
			else if((*gameboard[(this->getX()*WIDTH)+this->getY()-1]).getOccupant()->getType() == 'o'){
				Ant* temp = dynamic_cast<Ant*>((*gameboard[(this->getX()*WIDTH)+this->getY()-1]).getOccupant());
				delete (*gameboard[(this->getX()*WIDTH)+this->getY()-1]).getOccupant();
				(*gameboard[(this->getX()*WIDTH)+this->getY()-1]).setOccupant(this);
				this->setY(this->getY()-1);
				this->resetStarve();
				ateAnt = 1;
				(*gameboard[(this->getX()*WIDTH)+this->getY()+1]).setOccupant(NULL);
				break;
			} else
				;
		} else if(cnt%4 == 3){
			if(this->getX() == 0)
				;
			else if((*gameboard[((this->getX()-1)*WIDTH)+this->getY()]).getOccupant() == NULL)
				;
			else if((*gameboard[((this->getX()-1)*WIDTH)+this->getY()]).getOccupant()->getType() == 'o'){
				Ant* temp = dynamic_cast<Ant*>((*gameboard[((this->getX()-1)*WIDTH)+this->getY()]).getOccupant());
				delete (*gameboard[((this->getX()-1)*WIDTH)+this->getY()]).getOccupant();
				(*gameboard[((this->getX()-1)*WIDTH)+this->getY()]).setOccupant(this);
				this->setX(this->getX()-1);
				this->resetStarve();
				ateAnt = 1;
				(*gameboard[((this->getX()+1)*WIDTH)+this->getY()]).setOccupant(NULL);
				break;
			} else
				;
		} else
			;
	}
	if(ateAnt == 0)
		;
	else
		;

	if(ateAnt == 0){
		this->didntEat();
		if(this->getTimeTilStarve() == 0){
			(*gameboard[(this->getX()*WIDTH)+this->getY()]).setOccupant(NULL);
		}
		else{
			direction = rand() % 4;
			//Check surroundings
			for(int cnt(direction); cnt < (direction+4); cnt++){
				if(cnt%4 == 0){
					if(this->getY() == HEIGHT-1)
						;
					else if((*gameboard[(this->getX()*WIDTH)+this->getY()+1]).getOccupant() == NULL){
						(*gameboard[(this->getX()*WIDTH)+this->getY()+1]).setOccupant(this);
						this->setY(this->getY()+1);
						(*gameboard[(this->getX()*WIDTH)+this->getY()-1]).setOccupant(NULL);
						break;
					}
				} else if(cnt%4 == 1){
					if(this->getX() == WIDTH-1)
						;
					else if((*gameboard[((this->getX()+1)*WIDTH)+this->getY()]).getOccupant() == NULL){
						(*gameboard[((this->getX()+1)*WIDTH)+this->getY()]).setOccupant(this);
						this->setX(this->getX()+1);
						(*gameboard[((this->getX()-1)*WIDTH)+this->getY()]).setOccupant(NULL);
						break;
					}
				} else if(cnt%4 == 2){
					if(this->getY() == 0)
						;
					else if((*gameboard[(this->getX()*WIDTH)+this->getY()-1]).getOccupant() == NULL){
						(*gameboard[(this->getX()*WIDTH)+this->getY()-1]).setOccupant(this);
						this->setY(this->getY()-1);
						(*gameboard[(this->getX()*WIDTH)+this->getY()+1]).setOccupant(NULL);
						break;
					}
				} else if(cnt%4 == 3){
					if(this->getX() == 0)
						;
					else if((*gameboard[((this->getX()-1)*WIDTH)+this->getY()]).getOccupant() == NULL){
						(*gameboard[((this->getX()-1)*WIDTH)+this->getY()]).setOccupant(this);
						this->setX(this->getX()-1);
						(*gameboard[((this->getX()+1)*WIDTH)+this->getY()]).setOccupant(NULL);
						break;
					}
				} else
					;
			};
		}
	}
	return;
}

void Dbug::breed(){
	//Decrement timeToBreed
	if(this->getAge() < 1)
		;
	else if(this->getTimeTilSpawn() == 0)
		;
	else
		this->spawnCycle();
	bool hasBred = 0;
	//Check if needs to breed
	if(this->getTimeTilSpawn() == 0 && this->getAge() > 0){
		int direction = rand() % 4;
		for(int cnt(direction); cnt < (direction+4); cnt++){
			if(cnt%4 == 0){
				if(this->getY() == HEIGHT-1)
					;
				else if((*gameboard[(this->getX()*WIDTH)+this->getY()+1]).getOccupant() == NULL){
					Dbug* temp = new Dbug(this->getX(), this->getY()+1);
					hasBred = 1;
					(*gameboard[(this->getX()*WIDTH)+this->getY()+1]).setOccupant(temp);
					temp = NULL;
					break;
				}
			} else if(cnt%4 == 1){
				if(this->getX() == WIDTH-1)
					;
				else if((*gameboard[((this->getX()+1)*WIDTH)+this->getY()]).getOccupant() == NULL){
					Dbug* temp = new Dbug(this->getX()+1, this->getY());
					hasBred = 1;
					(*gameboard[((this->getX()+1)*WIDTH)+this->getY()]).setOccupant(temp);
					temp = NULL;
					break;
				}
			} else if(cnt%4 == 2){
				if(this->getY() == 0)
					;
				else if((*gameboard[(this->getX()*WIDTH)+this->getY()-1]).getOccupant() == NULL){
					Dbug* temp = new Dbug(this->getX(), this->getY()-1);
					hasBred = 1;
					(*gameboard[(this->getX()*WIDTH)+this->getY()-1]).setOccupant(temp);
					temp = NULL;
					break;
				}
			} else if(cnt%4 == 3){
				if(this->getX() == 0)
					;
				else if((*gameboard[((this->getX()-1)*WIDTH)+this->getY()]).getOccupant() == NULL){
					Dbug* temp = new Dbug(this->getX()-1, this->getY());
					hasBred = 1;
					(*gameboard[((this->getX()-1)*WIDTH)+this->getY()]).setOccupant(temp);
					temp = NULL;
					break;
				}
			} else
				;
		}
		if(hasBred == 1)
			this->resetTimeTilSpawn();
		else
			;
	} else
		;
	return;
}

void starveDbugs(){
	bool allAlive;
	if(Dbug::dbugCnt == 0)
		allAlive = 1;
	else
		allAlive = 0;
	while(allAlive == 0){
		for(int cnt(0); cnt < Dbug::dbugCnt; cnt++){
			if(Dbug::allDbugs[cnt]->getTimeTilStarve() == 0){
				delete Dbug::allDbugs[cnt];
				if(Dbug::dbugCnt == 0){
					allAlive = 1;
				} else
					;
				cnt--;
			} else
				allAlive = 1;
		}
	}
	return;
}
