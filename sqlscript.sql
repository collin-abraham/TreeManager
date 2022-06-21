CREATE TABLE trees (
	tree_id SERIAL PRIMARY KEY,
	tree_name VARCHAR(100) NOT NULL,
	tree_cost NUMERIC NOT NULL,
	quantity INTEGER NOT NULL,
	machine BOOLEAN NOT NULL
)

INSERT INTO trees 
(tree_name, tree_cost, quantity, machine)
VALUES
('Pine','350','63','TRUE')

INSERT INTO trees 
(tree_name, tree_cost, quantity, machine)
VALUES
('Oak','1000','150','TRUE')

INSERT INTO trees 
(tree_name, tree_cost, quantity, machine)
VALUES
('Willow','233','50','TRUE')

INSERT INTO trees 
(tree_name, tree_cost, quantity, machine)
VALUES
('Maple','110','300','TRUE')