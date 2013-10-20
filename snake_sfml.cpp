

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


#include <vector>
#include <iostream>
#include <math.h>


#define SNAKESZ 50
#define XRES 800
#define YRES 600
float TURN_SPEED=0.30;
const sf::Color plcolors[]={sf::Color(255,0,0),sf::Color(0,0,255),sf::Color(255,255,0),sf::Color(0,255,255)};


class Flag : public sf::CircleShape {
	sf::Vector2f base;
	
	public:
	Flag(sf::Color col,sf::Vector2f b) : sf::CircleShape(5),base(b) {
		setFillColor(col);
		reset();
	};
	
	void reset() {
		setPosition(base);
	};
};
struct Settings {
	float turnSpeed;
	sf::Keyboard::Key controlls[3][2]; //3 players have 2 buttons
	char winSize; //size of snake to win
	
	Settings() {
		turnSpeed=0.30;
		controlls[0][0]=sf::Keyboard::Left;
		controlls[0][1]=sf::Keyboard::Right;
			
		controlls[1][0]=sf::Keyboard::A;
		controlls[1][1]=sf::Keyboard::D;
			
		controlls[2][0]=sf::Keyboard::Numpad4;
		controlls[2][1]=sf::Keyboard::Numpad6;
		
		
	};
};

class Turner {
	sf::Keyboard::Key* controlls;
	
	public:
	Turner(sf::Keyboard::Key con[2]) : controlls(con) {};
	virtual float turn() {
		if (sf::Keyboard::isKeyPressed(controlls[0])) {
			return -TURN_SPEED; 
			};
		if (sf::Keyboard::isKeyPressed(controlls[1])) {
			return TURN_SPEED; 
			};
	}
};

	
	

class Snake {
	
	
	Turner* turner;
	sf::Keyboard::Key *controlls;
	std::vector<sf::Vector2f> snake;
	sf::Vector2f movedir,start_pos;
	bool has_eaten; //if true,elongate the snake.
	int start_size;
	
	float turn_radian,start_radian;
	
	sf::Shape* inventory; //carriable object
	
	public:
	static char snakenum;
	
	char id;
	int score;
	
	//rotation is clock-wise
	Snake(int start,float start_dir,sf::Vector2f start_pos,Turner* t):turner(t),inventory(NULL) {
		id=snakenum++;
		start_size=start;
		score=0;
		turn_radian=start_radian=start_dir;
		
		this->start_pos=start_pos;
		reset();
	};
	
	void reset() {
		has_eaten=false;
		turn_radian=start_radian;
		//using movedir as paceholder
		movedir = sf::Vector2f(10*cos(start_radian),10*sin(start_radian));//30,0);
		snake = std::vector<sf::Vector2f>();
		sf::Vector2f point = start_pos;
		for (int i=0 ;   (i<start_size) ; i++) {
			
			snake.push_back(point);
			point+=movedir;
		};
		inventory=NULL;
		
	};
	bool win() { return snake.size()==SNAKESZ; };
	
	void move(std::vector<Snake>& snakes) {
	
		
		int size = snake.size();
		sf::Vector2f t,old=snake[size-1];
		
		
		if (inventory!=NULL) {
			inventory->setPosition(snake[size-1]+sf::Vector2f(15,15));
		};
		if (has_eaten) {
			snake.push_back(snake[size-1]+movedir);
			has_eaten=false;
		} else
			
			//this shit's retarded yo - beware or refactor.
			for (int i = size-1 ; i >=0 ; i--) {
				if (i==size-1) {
					snake[size-1]+=movedir;
					
				//collisions and frags
					for (int sn=0;sn <snakes.size();sn++) {
						for (int y=0;y<=snakes[sn].snake.size();y++) {
							//If we're going through ourselves and we're at the head,skip collission
							if ((snakes[sn].id==id) && (y==size-1)) continue;
							if (sf::FloatRect(snakes[sn].snake[y],sf::Vector2f(20,20)).contains(snake[size-1]+movedir)) {
								
								//std::cout << std::to_string(id) << " crash " << std::to_string(snake2.id) << " " <<  std::to_string(y) << "\n";
							
								if (id!=snakes[sn].id) {snakes[sn].score+=1;}
								else score--;
								reset();
								return;
							};
						}
					};
				}
				else {
					t=snake[i];
					snake[i]=old;
					old=t;
				};
				
				//wrap-around
				if (snake[i].x<-15) 
					snake[i].x=XRES-30;
				else if (snake[i].x>XRES)
					snake[i].x=-15;
				
				if (snake[i].y<-15) 
					snake[i].y=YRES-30;
				else if (snake[i].y>YRES)
					snake[i].y=-15;	
			};
	};
	
	bool try_eat(sf::Shape& p) {
		has_eaten=collide(p);
		if (has_eaten) score++;
		return has_eaten;
	};
	void turn() {
		turn_radian+=turner->turn();
		movedir=sf::Vector2f(10*cos(turn_radian),10*sin(turn_radian));
	};
	
	bool collide(sf::Shape& obj) {
		sf::FloatRect head(snake[snake.size()-1],sf::Vector2f(30,30));
		return head.intersects(obj.getGlobalBounds());
	};
		
	void draw(sf::RenderWindow& win) {
		sf::CircleShape point(15);
		
		point.setFillColor(plcolors[id]); //Warning-out of bounds error when there more than four players
		
		int size = snake.size();
		for (int i=0;i<size;i++) {
			
			point.setPosition(snake[i]);
			win.draw(point);
		};
		
	};
	/* //double free runtime bug
	~Snake() { 
		delete turner;
	};
	//*/
	
	void carry(sf::Shape& obj) {inventory=&obj;};
	bool isCarrying() {return inventory!=NULL;};
	void drop() {inventory=NULL;};
};
char Snake::snakenum=0;		
			
			
	
		

int main(int argc, char** argv)
{
	
	
	Settings settings=Settings();	
	std::vector<Snake> players;
	sf::Text scores;
	Flag flags[2]={Flag(sf::Color::Blue,sf::Vector2f(XRES-50,YRES/2)),Flag(sf::Color::Red,sf::Vector2f(50,YRES/2))};
	
	flags[0].setFillColor(sf::Color::Blue);
	flags[1].setFillColor(sf::Color::Red);
	
	
	scores.setPosition(0,0);
	players.push_back(Snake(5,0,sf::Vector2f(15,15),new Turner(settings.controlls[0])));
	players.push_back(Snake(5,-3.1428,sf::Vector2f(XRES-15,YRES-45),new Turner(settings.controlls[1])));

	
	std::srand(time(NULL));
	
	sf::RectangleShape food(sf::Vector2f(20,20));
	food.setPosition(sf::Vector2f(15*(std::rand()%(XRES/15)),15*(std::rand()%(YRES/15))));
	food.setFillColor(sf::Color::Green);
	
	int difficulty=40;
	
	
	
	sf::RenderWindow win(sf::VideoMode(XRES,YRES,32),"Snake");
	win.setFramerateLimit(difficulty);
	sf::Event mainloop;
	
	while (win.isOpen()) {
		win.clear();
		
		
		for (int i=0;i<players.size();i++)
		{
			bool eaten=false;
			players[i].turn();
			eaten= eaten || players[i].try_eat(food);
		
			if (eaten) {
				food.setPosition(sf::Vector2f(15*(std::rand()%(XRES/15))+5,15*(std::rand()%(YRES/15))+5));
			};
		}
		win.draw(food);
		std::string scorestr="";
		
		
		for (int i=0;i<players.size();i++) {
			players[i].draw(win);
			for (int y=0;y<2;y++) {
				if (players[i].collide(flags[y])) {
					if (i!=y) {
						if (players[i].isCarrying()) {
							  flags[y].reset();
							  players[i].drop();
							  players[i].score++;
							  
						};
					
				}
				else players[i].carry(flags[y]);
				};
				win.draw(flags[y]);
			};
			
			
			scorestr+="P"+std::to_string(players[i].id+1)+": "+std::to_string(players[i].score)+"\n";
		};
		scores.setString(scorestr);
		win.draw(scores);
		
		
		while (win.pollEvent(mainloop)) {
			
			if (mainloop.type==sf::Event::Closed) win.close();
			
			if (mainloop.type==sf::Event::KeyPressed) {
				
				
				switch (mainloop.key.code) {
					case sf::Keyboard::Key::Add  : {TURN_SPEED+=0.05;break;}
					case sf::Keyboard::Key::Subtract   : {TURN_SPEED-=0.05;break;}
					default: break;
				};
				
				
			};
		};
		
		win.display();
		for (int i=0;i<players.size();i++) players[i].move(players);
		
	};
		
	return 0;
}
