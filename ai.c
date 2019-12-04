#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>


#include "ai.h"
#include "utils.h"
#include "priority_queue.h"

extern int tot_generated;
extern int tot_expanded;
extern int all_max_depth;
struct heap h;

float get_reward( node_t* n );

/**
 * Function called by pacman.c
*/
void initialize_ai(){
	heap_init(&h);
}

//check if move 1 is prependicular to the move 2
bool prependicular(move_t move_1, move_t move_2){
	return ((move_1==left || move_1==right) && (move_2==up || move_2==down))
		||((move_2==left || move_2==right) && (move_1==up || move_1==down));
}

/**
 * function to copy a src into a dst state
*/
void copy_state(state_t* dst, state_t* src){
	//Location of Ghosts and Pacman
	memcpy( dst->Loc, src->Loc, 5*2*sizeof(int) );

    //Direction of Ghosts and Pacman
	memcpy( dst->Dir, src->Dir, 5*2*sizeof(int) );

    //Default location in case Pacman/Ghosts die
	memcpy( dst->StartingPoints, src->StartingPoints, 5*2*sizeof(int) );

    //Check for invincibility
    dst->Invincible = src->Invincible;

    //Number of pellets left in level
    dst->Food = src->Food;

    //Main level array
	memcpy( dst->Level, src->Level, 29*28*sizeof(int) );

    //What level number are we on?
    dst->LevelNumber = src->LevelNumber;

    //Keep track of how many points to give for eating ghosts
    dst->GhostsInARow = src->GhostsInARow;

    //How long left for invincibility
    dst->tleft = src->tleft;

    //Initial points
    dst->Points = src->Points;

    //Remiaining Lives
    dst->Lives = src->Lives;

}

node_t* create_init_node( state_t* init_state ){
	node_t * new_n = (node_t *) malloc(sizeof(node_t));
	new_n->parent = NULL;
	new_n->priority = 0;
	new_n->depth = 0;
	new_n->num_childs = 0;
	copy_state(&(new_n->state), init_state);
	new_n->acc_reward =  get_reward( new_n );
	return new_n;

}


float heuristic( node_t* n ){
	float h = 0;
	float i,l,g;
	//See if pacman will be invincible
	i = 0;
	l = 0;
	if (n->parent!=NULL){
		i = (n->state.Invincible && !(n->parent->state.Invincible))*INVINCIBLE;
		//Check if life is lost in the state
		l = (n->parent->state.Lives > n->state.Lives)*LIFE_LOST;
	}
	//Check if game is over in that state
	g = (n->state.Lives == -1)*GAME_OVER;
	h = i-l-g;
	return h;
}

float get_reward ( node_t* n ){
	float reward = 0;

	//Adding up the numbers as the formula: r(n)=[h(n)+s(n)-s(nParent)]*0.99^depth
	reward += heuristic(n); // h(n)
	reward += n->state.Points; // s(n)
	if (n->parent!=NULL){
		reward -= n->parent->state.Points; // s(nParent)
	}
	float discount = pow(0.99,n->depth); // 0.99^depth

	return discount * reward;
}

/**
 * Apply an action to node n and return a new node resulting from executing the action
*/
bool applyAction(node_t* n, node_t** new_node, move_t action ){

	bool changed_dir = false;

	*new_node = (node_t *) malloc(sizeof(node_t));
	(*new_node)->depth = n->depth++;
	(*new_node)->priority = n->depth;
  

  (*new_node)->num_childs = 0;
  (*new_node)->move = action;
  copy_state(&(*new_node)->state, &(n->state));
  (*new_node)->parent=n;

  changed_dir = execute_move_t( &((*new_node)->state), action );

	(*new_node)->acc_reward = n->acc_reward + get_reward(*new_node);

	return changed_dir;

}

// return the first node move that the node belongs to
node_t* propagateBack( node_t* n ){
	while(n->parent->parent!=NULL){
		n=n->parent;
	}
return n;
}

/**
 * Find best action by building all possible paths up to budget
 * and back propagate using either max or avg
 */

// Function is added the recent_move input to find a more suitable path

/**Logic for this is: Between 3 options with same score, priority of choice is:
	*	_Prependicular to recent_move
	*	_Same as recent_move
	*/
move_t get_next_move( state_t init_state, int budget, propagation_t propagation, char* stats, move_t recent_move){

	float best_action_score[SIZE];
	for(unsigned i = 0; i < SIZE; i++){
	    best_action_score[i] = INT_MIN;
	} // action not used initially, set them to be negative
	move_t best_action = left; // default best_action will be left

	unsigned generated_nodes = 0;
	unsigned expanded_nodes = 0;
	unsigned max_depth = 0;



	//Add the initial node
	node_t* n = create_init_node( &init_state ); // start

	//Use the max heap API provided in priority_queue.h
	heap_push(&h,n); // priorityQueue containing n only

	//FILL IN THE GRAPH ALGORITHM
	node_t* new_node;
	node_t* *explored =(node_t**) malloc(sizeof(node_t*)*budget); // empty array

	while (h.count!=0){
		n = heap_delete(&h); // node<-frontier.pop()

		//explored.add(n)

		if (expanded_nodes < budget){
			explored[expanded_nodes] = n;
			expanded_nodes++;
			// for each applicable action
			//node_t *max_node = NULL;
			for (unsigned i = 0; i < SIZE; i++){


				if(applyAction(n, &(new_node), i)){	//new_node <- applyAction
					generated_nodes++;
					max_depth = new_node->depth;
					node_t* first = propagateBack(new_node); // see what move is the first move of node
					first->num_childs++;

					//If an action is applicable from the start, it should return correct value
					if(best_action_score[first->move]==INT_MIN){
						best_action_score[first->move]=0;
					}

					// propagateBackScoreToFirstAction(new_node, propagation)
					// is it max or avg?
					if (propagation == max){

						// see if the new acc_reward is bigger than current max
						if(new_node->acc_reward > best_action_score[first->move]){
							best_action_score[first->move]=new_node->acc_reward;
						}
					}

					else if (propagation == avg){

						// average=previous*num_childs/(num_childs-1)
						float unormalised=best_action_score[first->move] * first->num_childs + get_reward(new_node);
						best_action_score[first->move]=unormalised/(first->num_childs+1);

					}

					// if lost life
					if (new_node->parent->state.Lives - new_node->state.Lives == 1){
						// delete new_node
						new_node->parent=NULL;
						free(new_node);
						new_node = NULL;
					}
					else {
						heap_push(&h, new_node); //frontier.add(new_node)
					}
				}
				else{
					//free the new node
					new_node->parent=NULL;
					free(new_node);
				}
			}
		}
		else{
			//free the heap and the final popped node
			emptyPQ(&h);
			free(n);
		}

	}

	for(unsigned i=0; i<expanded_nodes;i++){
		free(explored[i]);
	}
	free(explored); //free memory

	for (unsigned i = 0; i < SIZE; i++){

		unsigned tie_break = rand() % 2; //tie break randomly

		//add "&& best_action_score[i]!=INT_MIN" for guarding out inapplicable cases
		//case 1: better reward
		if (best_action_score[i]>best_action_score[best_action] && best_action_score[i]!=INT_MIN){
			best_action = i;
		}

		//case 2: equal reward
		else if(best_action_score[i]==best_action_score[best_action] && best_action_score[i]!=INT_MIN){

			// case 2.1: movement i prependicular to recent move: make a turn
			if(prependicular(recent_move, i)){

				// case 2.1.1: current best move is also prependicular to recent move: random turn then
				if (prependicular(recent_move, best_action)){
					if(tie_break){
						best_action = i;
					}
				}
				// case 2.1.2: current best move is not prependicular to recent move: make the turn
				else {
					best_action = i;
				}
			}
			// case 2.2: movement not prependicular: follow the recent move
			else {
				best_action = recent_move;
			}
		}
	}

	// add to global variables
	tot_generated+=generated_nodes;
	tot_expanded+=expanded_nodes;
	if(max_depth>all_max_depth){
		all_max_depth=max_depth;
	}

	sprintf(stats, "Max Depth: %d Expanded nodes: %d  Generated nodes: %d\n",max_depth,expanded_nodes,generated_nodes);
	if(best_action == left)
		sprintf(stats, "%sSelected action: Left\n",stats);
	if(best_action == right)
		sprintf(stats, "%sSelected action: Right\n",stats);
	if(best_action == up)
		sprintf(stats, "%sSelected action: Up\n",stats);
	if(best_action == down)
		sprintf(stats, "%sSelected action: Down\n",stats);

	sprintf(stats, "%sScore Left %f Right %f Up %f Down %f",stats,best_action_score[left],best_action_score[right],best_action_score[up],best_action_score[down]);
	return best_action;
}
