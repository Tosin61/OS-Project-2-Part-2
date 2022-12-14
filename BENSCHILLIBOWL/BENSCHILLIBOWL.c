#include "BENSCHILLIBOWL.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

bool IsEmpty(BENSCHILLIBOWL* bcb);
bool IsFull(BENSCHILLIBOWL* bcb);
void AddOrderToBack(Order **orders, Order *order);

MenuItem BENSCHILLIBOWLMenu[] = { 
    "BensChilli", 
    "BensHalfSmoke", 
    "BensHotDog", 
    "BensChilliCheeseFries", 
    "BensShake",
    "BensHotCakes",
    "BensCake",
    "BensHamburger",
    "BensVeggieBurger",
    "BensOnionRings",
};
int BENSCHILLIBOWLMenuLength = 10;

/* Select a random item from the Menu and return it */
MenuItem PickRandomMenuItem() {
    int randomNum = rand() % BENSCHILLIBOWLMenuLength;
    return BENSCHILLIBOWLMenu[randomNum];
}

/* Allocate memory for the Restaurant, then create the mutex and condition variables needed to instantiate the Restaurant */

BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
    printf("Restaurant is open!\n");
    BENSCHILLIBOWL *bens_chili_bowl = (BENSCHILLIBOWL*) malloc(sizeof(BENSCHILLIBOWL));

    bens_chili_bowl->current_size=0;
    bens_chili_bowl->max_size=max_size;
    bens_chili_bowl->orders = NULL;
    bens_chili_bowl->next_order_number=1;
    bens_chili_bowl->expected_num_orders=expected_num_orders;
    bens_chili_bowl->orders_handled=0;

    pthread_mutex_init(&(bens_chili_bowl->mutex), NULL);
    pthread_cond_init(&(bens_chili_bowl->can_add_orders), NULL);
    pthread_cond_init(&(bens_chili_bowl->can_get_orders), NULL);
    return bens_chili_bowl;
}


/* check that the number of orders received is equal to the number handled (ie.fullfilled). Remember to deallocate your resources */

void CloseRestaurant(BENSCHILLIBOWL* bcb) {
    if (bcb->expected_num_orders == bcb->orders_handled){
        pthread_mutex_destroy(&(bcb->mutex));
        free(bcb);
        printf("Restaurant is closed!\n");
    }  
    else {
        printf("Missing orders");
    }
}

/* add an order to the back of queue */
int AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
    pthread_mutex_lock(&(bcb->mutex));
  while(IsFull(bcb)){
    pthread_cond_wait(&(bcb->can_add_orders), &(bcb->mutex));
  }
  
  order->order_number = bcb->next_order_number;
  AddOrderToBack(&(bcb->orders), order);
  return order->order_number;
}

/* remove an order from the queue */
Order *GetOrder(BENSCHILLIBOWL* bcb) {
    pthread_mutex_lock(&(bcb->mutex));
  while (IsEmpty(bcb)){
    if (bcb->orders_handled>=bcb->expected_num_orders){
      pthread_mutex_unlock(&(bcb->mutex));
      return NULL;
    }
    pthread_cond_wait(&(bcb->can_get_orders), (&bcb->mutex));
  }
  
  Order *order = bcb->orders;
  bcb->orders = bcb->orders->next;
  bcb->current_size -= 1;
  bcb->orders_handled += 1;
  
  pthread_cond_broadcast(&(bcb->can_add_orders));
  pthread_mutex_unlock(&(bcb->mutex));
  
  return order;
}

// Optional helper functions (you can implement if you think they would be useful)
bool IsEmpty(BENSCHILLIBOWL* bcb) {
  if (bcb->current_size == 0){
        return true;
    }
    else {return false;}
}

bool IsFull(BENSCHILLIBOWL* bcb) {
    if (bcb->current_size == bcb->max_size){
        return true;
    }
    else {return false;}
}

/* this methods adds order to rear of queue */
void AddOrderToBack(Order **orders, Order *order) {
    if (*orders == NULL){
        *orders = order;
    }
    else{
        Order *curr_order;
        while (curr_order->next){
            curr_order = curr_order->next;
        }
        curr_order->next = order;
    }
}

