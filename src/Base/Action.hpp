//
// Created by varun on 1/31/2025.
//

#ifndef ACTION_HPP
#define ACTION_HPP


class Action {
public:
	void (*action)(void *, void *);
	void *sParam;
	void *oParam;
	int (*getWeight)(void *, void *);

	Action(void (*action)(void *, void *), void(*sParam), void(*oParam), int (*getWeight)(void *, void *)) {
		this->action = action;
		this->sParam = sParam;
		this->oParam = oParam;
		this->getWeight = getWeight;
	}

	~Action() = default;
};


#endif // ACTION_HPP
