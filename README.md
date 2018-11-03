# HillClimbing

N-Queen Problem Formulation:  
----------------------------  
  • Initial State       – Randomly generated arrangement of 0 to N queens on the board.  
  • Successor Function  – Generates successors for the N queen board and stores their collision information  
  • Goal Test           – N queens are on the board, none attacked.  
  • Path Cost           – number of collisions for that particular queen, which is nothing but the heuristic cost.  

Hill Climbing variants implemented:
-----------------------------------
  1) Steepest Ascent
  2) Sideways Move
  3) Random Restart without sideways move
  4) Random Restart with sideways move

Functionality:  
---------------------------------------------------------------------------------------------------------------------------------------- 
    Hill Climbing algorithm implemented here in general generates a random configuration and it will be set as the 
    board’s initial configuration. It will get the current board’s heuristic cost and saves it in a local variable.
    It then generates the successors based on the board size provided by the user. Successors in the sense it will 
    calculate each row & column collision cost.
    
    Then it gets the least collisions from the generated set. Now, the comparison between the current board’s 
    collision value and the least collision value among the set depends on the hill climbing variant we choose.
    
    If there are any least collision successors available, it stores that information in a vector. From the vector 
    we choose any random successor to proceed further. If we don’t choose randomly from the successors list, there 
    might be a chance it will loop indefinetly among any 2 successors until the limit is reached. To avoid that, we
    choose successor randomly.

    If the chosen successor has ‘0’ collisions, then we have reached the goal state having queens placed on the board
    without attacking each other. If not, it will continue generating successors from the randomly chosen successor.
    
    These are the hill climbing variants we have implemented. The source code and the corresponding results having 
    said that we executed every variant 100, 200, 300, 400 & 500 times to calculate the success percentage, average
    success & failure steps.
    
    1) Steepest Ascent hill climbing algorithm is very simple but suffers from 2 issues. Local minima or plateaus,
       if it encounters any one of those it will return false. This is the reason we have a probability of 14% 
       success every time.

    2) Sideways Move hill climbing algorithm rectifies the above problem. If the algorithm finds any plateaus instead
       of returning, we keep a limit of 100 to consider the same collision cost and to continue from that. This way, 
       if it encounters a shoulder it should get a local maxima and will continue. Because of this approach, the 
       probability of success increased to 94%.
    
    3) Random Restart hill climbing approach is way better than the above 2 variants. Here, whenever the algorithm 
       reaches the plateau or local minima, it generates a random board and continues the algorithm considering that 
       as the current board. This way the success probability has increased to 100% for both ‘without sideways’ & 
       ‘with sideways’ approach. But anyways the number of random restarts and average number of steps taken would 
       vary for both.

Results:
---------------------------------
1) Steepest Ascent  
	a) average success count  	        4  
	b) average failure count                3  
	c) success %      			14%

2) Sideways move  
	a) average success count  	        21  
	b) average failure count                64  
	c) success %      			94%  

3) Random Restart without sideways  
	a) random restarts 			 7  
	b) steps				22  

4) Random Restart with sideways:  
	a) random restarts 			 1  
	b) steps				25  
