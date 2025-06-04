Suponha que você foi contratado por uma pequena loja de produtos de surf Tati Surf Co.MMM

Seu programa deverá fazer a gestão completa desde o recebimento dos produtos, a revenda e saída para entrega.
Considere que os produtos vendidos nesta loja são: Parafina, Leash (cordinha de segurança), Quilhas e Decks Antiderrapantes.
Os produtos são gerenciados através dos seguintes dados:

- [x] 1. Código do produto (código único para cada unidade de cada produto da loja) [cite: 3]
- [x] 2. Tipo do produto (Parafina, Leash, Quilha, Deck) [cite: 3]
- [x] 3. Descrição do produto: marca do produto e outras informações que podem ser relevantes para o cliente. [cite: 3]
- [x] 4. Preço [cite: 4]

**PROCESSO DE RECEBIMENTO DE PRODUTOS - ENTREGA 08/06**

- [x] Todos os dias pela manhã é possível que seja recebido produtos novos na loja para revenda, caso isso aconteça, você deve adicionar os produtos na lista de produtos disponíveis para venda e também em pilhas no estoque.

- [x] **Lista de produtos disponíveis para venda**

  - [x] Você deve criar uma lista duplamente encadeada de produtos disponíveis para venda. [cite: 5]
  - [x] Ou seja, cada nó da lista deve conter os dados do produto conforme mencionado acima e a lista deve sempre estar ORDENADA pelo preço dos produtos em ordem crescente. [cite: 6]
  - [x] ATENÇÃO: quando um produto chega na loja, você deve adicionar na lista e automaticamente a função de adicionar deve conter uma lógica para que o produto seja adicionado em uma posição de forma a manter sempre a lista ordenada conforme mencionado, ou seja, você não deve adicionar por posição, quem irá indicar a posição da lista é o preço do produto. [cite: 7]
  - [x] Se houver mais de um produto com o mesmo preço, considere a ordem de quem chegou primeiro. [cite: 8]

- [x] **Pilha de estoque**
  - [x] Suponha que há 4 pilhas de caixas no estoque da loja, uma pilha para cada categoria de produto. [cite: 9]
  - [x] Cada caixa cabe uma quantidade de produtos específicos:
    - [x] 1. Caixa de parafinas: até 50 unidades [cite: 10]
    - [x] 2. Caixa de leash: até 25 unidades [cite: 10]
    - [x] 3. Caixa de quilhas: até 10 unidades [cite: 10]
    - [x] 4. Caixa de decks: até 5 unidades [cite: 10]
  - [x] Para simular essa situação, você deve criar também quatro pilhas de caixas, uma para cada categoria, onde cada caixa de cada categoria deve conter:
    - [x] 1. Código da caixa (valor único para cada caixa criada)
    - [x] 2. Lista encadeada de códigos dos produtos que estão nesta caixa
    - [x] 3. Quantidade de unidades do produto que estão na caixa
  - [x] ATENÇÃO: quando chegar novos produtos, você deve adicionar na pilha certa para aquele produto, mas antes, você deve verificar quantos unidades tem na caixa do topo da pilha da categoria do produto que está sendo recebido: [cite: 11]
    - [x] Se a caixa já está com o limite máximo de unidades, você deve criar uma nova caixa e adicionar as novas unidades que chegaram. [cite: 11]
    - [x] Se a caixa não está no limite, você deve completar primeiro as unidades que cabem nessa caixa e somente se for necessário (dependendo da quantidade de unidades que foram recebidas) criar uma nova caixa, atualizá-las com os dados dos produtos que chegaram que ainda restaram e adicioná-la no topo da pilha. [cite: 12]

---

**PROCESSO DE REVENDA - ENTREGA 22/06**

- [ ] Você deve criar uma função para visualização dos produtos a serem vendidos, o cliente tem a seguintes opções: [cite: 13]
  - [ ] 1. Ver produtos por categoria (Parafina, Leash, Quilha, Deck): dado como parâmetro a categoria, apresentar todos os produtos em ordem crescente de preço. [cite: 13]
  - [ ] 2. Ver produtos dentro de um intervalo de preço: dado como parâmetro um limite inferior e superior de preços, você deve apresentar todos os produtos dentro deste limite. [cite: 14]
- [ ] Você também deve criar uma função para compra de um produto [cite: 15]
  - [ ] 1. Comprar produto: dado como parâmetro o código do produto, você deve inicialmente: [cite: 15]
    - [ ] a. Remover o produto da pilha no estoque: para isso você deverá inicialmente encontrar o produto na pilha da sua categoria, ou seja, você deve remover a caixa do topo e procurar na lista de códigos o produto que será vendido, caso não esteja nesta caixa, você deverá colocá-la em uma pilha auxiliar, procurar na próxima caixa do topo, etc, até encontrar. [cite: 16]
    - [ ] Quando encontrar, você deve: empilhar novamente todas caixas que estão na pilha auxiliar novamente na pilha de produtos da categoria em questão, com exceção da última caixa, pois essa você deve usar para equilibrar novamente a caixa que você removeu o produto que será vendido, para isso, você deve passar um produto da última caixa da pilha auxiliar para a caixa onde o produto foi removido, depois você deve adicionar a caixa equilibrada de onde o produto foi removido na pilha de produtos da categoria em questão, e por fim a caixa que foi usada para a operação de equilíbrio (última caixa que estava na pilha auxiliar). [cite: 17, 18]
    - [ ] b. Remover o produto da lista de produtos disponíveis para venda e criar o processo de venda: aqui basta remover o produto da lista duplamente encadeada através do código do produto, garanta que essa função retorna os dados do pedido removido, pois estes dados serão necessários para o próximo processo. [cite: 19]

---

**PROCESSO DE ENTREGA E PAGAMENTO**

**ENTREGA 06/07** [cite: 20]

- [ ] Considere que os pedidos e pagamento serão realizados por um conjunto de 3 entregadores, eles entregam o produto e recebem o pagamento. [cite: 20]
- [ ] Para realizar a entrega você deverá preparar o pedido da seguinte forma: [cite: 21]
  - [ ] Receber os dados do produto (retorno da função de remoção da lista de produtos disponíveis para venda) e usar esses dados para criar um pedido de entrega (que será um outro nó, que desta vez irá para uma fila): este pedido deverá conter além dos dados do produto removido da lista, o nome do cliente, CPF, CEP (somente números), nome da rua, número da casa ou apartamento e complemento. [cite: 21]
  - [ ] Assim que criar o nó você deve adicioná-lo em uma fila que irá simular os pedidos para entrega. [cite: 22]
- [ ] Por fim, você também deve criar uma função para remover da fila, que apresenta os dados do produto que está sendo removido no início da fila de entrega, ou seja, para simular a situação que o entregador chega na loja e você entrega o próximo produto da fila que deve ser entregue, você deve apenas imprimir o texto: "Produto saindo para entrega... e os dados do produto/cliente que está saindo da fila. [cite: 23]

**REGRAS CADA ETAPA DEVE SER TESTADA DA SEGUINTE FORMA:**

- [ ] **PROCESSO DE RECEBIMENTO DE PRODUTOS**

  - [ ] Você deve adicionar 100 produtos para venda.
  - [ ] Crie funções que imprimam os dados que estão nas 4 pilhas do estoque para verificação da corretude delas. [cite: 24]

- [ ] **PROCESSO DE REVENDA**

  - [ ] Simule a visualização/compra de pelo menos 20 produtos distintos.

- [ ] **PROCESSO DE ENTREGA E PAGAMENTO**
  - [ ] Simule a entrega de pelo menos 10 produtos vendidos.
