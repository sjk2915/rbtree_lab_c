#include <stdio.h>
#include <stdbool.h>

#define MAX_NODES 501

typedef enum { COLOR_RED, COLOR_BLACK } color_t;

typedef struct node_t
{
	int key;
	color_t color;
	struct node_t *left, *right;
} node_t;

node_t new_node(int key)
{
	node_t node;
	node.key = key;
	node.left = NULL;
	node.right = NULL;
	return node;
}

typedef struct bh_state
{
	bool is_possible;
	bool possible_bh[MAX_NODES + 2];
} bh_state;

node_t tree[MAX_NODES];
bh_state dp_red[MAX_NODES];
bh_state dp_black[MAX_NODES];
int p[MAX_NODES];
int n;

void post_order_dp(int node)
{
	// 기저 사례: 리프 노드
	if (tree[node].left == NULL && tree[node].right == NULL)
	{
		// 빨강으로 칠하기
		dp_red[node].is_possible = true;
		dp_red[node].possible_bh[1] = true;
		// 검정으로 칠하기
		dp_black[node].is_possible = true;
		dp_black[node].possible_bh[2] = true;
		return;
	}

	// 바텀 업
	if (tree[node].left != NULL) post_order_dp(tree[node].left->key);
	if (tree[node].right != NULL) post_order_dp(tree[node].right->key);

	// 본문
	bool left_possible = tree[node].left == NULL || dp_red[tree[node].left->key].is_possible || dp_black[tree[node].left->key].is_possible;
	bool right_possible = tree[node].right == NULL || dp_red[tree[node].right->key].is_possible || dp_black[tree[node].right->key].is_possible;
	// 왼쪽 서브트리가 유효하지 않거나
	// 오른쪽 서브트리가 유효하지 않으면 return
	if (!left_possible && !right_possible) return;

	// 빨강으로 칠하기
	for (int bh = 2; bh <= n + 2; bh++)
	{
		// 빨강 노드의 자식 노드는 bh가 부모와 같음
		bool left_possible = (tree[node].left == NULL) ? false : dp_black[tree[node].left->key].possible_bh[bh];
		bool right_possible = (tree[node].right == NULL) ? false : dp_black[tree[node].right->key].possible_bh[bh];

		if (left_possible && right_possible)
		{
			dp_red[node].is_possible = true;
			dp_red[node].possible_bh[bh] = true;
		}
	}
	
	// 검정으로 칠하기
	for (int bh = 2; bh <= n + 2; bh++)
	{
		// 검정 노드의 자식 노드는 bh가 부모의 bh-1
		bool left_possible = (tree[node].left == NULL) ? (bh == 2) : (dp_red[tree[node].left->key].possible_bh[bh-1] || dp_black[tree[node].left->key].possible_bh[bh-1]);
		bool right_possible = (tree[node].right == NULL) ? (bh == 2) : (dp_red[tree[node].right->key].possible_bh[bh-1] || dp_black[tree[node].right->key].possible_bh[bh-1]);

		if (left_possible && right_possible)
		{
			dp_black[node].is_possible = true;
			dp_black[node].possible_bh[bh] = true;
		}
	}
}

void coloring(int node, int required_bh)
{
	// 검정으로 칠하기
	if (dp_black[node].possible_bh[required_bh])
	{
		tree[node].color = COLOR_BLACK;

		if (tree[node].left != NULL) coloring(tree[node].left->key, required_bh - 1);
		if (tree[node].right != NULL) coloring(tree[node].right->key, required_bh - 1);
		return;
	}

	// 빨강으로 칠하기
	if (dp_red[node].possible_bh[required_bh])
	{
		tree[node].color = COLOR_RED;

		if (tree[node].left != NULL) coloring(tree[node].left->key, required_bh);
		if (tree[node].right != NULL) coloring(tree[node].right->key, required_bh);
		return;
	}
}

int main(void)
{
	scanf("%d", &n);
	for (int i = 1; i <= n; i++)
		scanf("%d", &(p[i]));
	for (int i = 1; i <= n; i++)
		tree[i] = new_node(i);
	for (int i = 1; i <= n; i++)
	{
		int parent = p[i];
		if (parent == 0) continue;
		else if (tree[parent].left == NULL)
			tree[parent].left = &tree[i];
		else if (tree[parent].right == NULL)
			tree[parent].right = &tree[i];
	}

	int root = 0;
	for (int i = 1; i <= n; i++)
	{
		if (p[i] == 0)
		{
			root = i;
			break;
		}
	}

	post_order_dp(root);

	if (dp_red[root].is_possible || dp_black[root].is_possible)
	{
		for (int bh = 1; bh <= n + 2; bh++)
		{
			if (dp_red[root].possible_bh[bh] || dp_black[root].possible_bh[bh])
			{
				coloring(root, bh);
				break;
			}
		}

		for (int i = 1; i <= n; i++)
		{
			if (tree[i].color == COLOR_RED)
				printf("R");
			else
				printf("B");
		}
		printf("\n");
	}
	
	else
	{
		printf("Impossible\n");
	}

	return 0;
}