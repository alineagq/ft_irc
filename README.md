# ft_irc

# Configuração Inicial e Estrutura do Projeto

**Descrição:**
- Crie a estrutura básica do projeto para o servidor IRC.
- Configure o Makefile com as regras `$(NAME)`, `all`, `clean`, `fclean`, e `re`.
- Implemente a configuração inicial do servidor para aceitar conexões.
- Garanta que o código seja compilável com C++ 98 usando as flags `-Wall -Wextra -Werror`.

**Tarefas:**
- Criar a estrutura de diretórios e arquivos iniciais (`src`, `include`, `Makefile`, etc.).
- Implementar a função principal que inicializa o servidor e configura o `socket`.
- Garantir que o servidor escute em uma porta especificada e aceite conexões.

**Critérios de Aceitação:**
- O projeto deve compilar sem erros.
- O servidor deve iniciar e escutar na porta fornecida.

# Implementação de Autenticação e Configuração de Usuários

**Descrição:**
- Implemente o mecanismo de autenticação dos clientes usando a senha fornecida.
- Permita que clientes configurem seu apelido (nickname) e nome de usuário após a conexão.

**Tarefas:**
- Adicionar lógica para verificar a senha de conexão.
- Implementar a configuração de apelido e nome de usuário.
- Validar que as informações do cliente sejam corretamente armazenadas e utilizadas.

**Critérios de Aceitação:**
- O servidor deve autenticar clientes com a senha fornecida.
- Clientes devem ser capazes de definir e alterar seu apelido e nome de usuário.

# Implementação de Gerenciamento de Canais e Mensagens

**Descrição:**
- Desenvolva a funcionalidade para criação, entrada e saída de canais.
- Implemente o envio e recebimento de mensagens em canais.
- As mensagens enviadas para um canal devem ser distribuídas para todos os membros do canal.

**Tarefas:**
- Criar lógica para os clientes se juntarem e saírem de canais.
- Implementar o envio e recebimento de mensagens de canais.
- Garantir que as mensagens enviadas por um cliente sejam encaminhadas para todos os clientes no canal.

**Critérios de Aceitação:**
- Clientes devem conseguir criar, ingressar e sair de canais.
- Mensagens enviadas para um canal devem ser visíveis para todos os membros do canal.

# Implementação de Funcionalidades de Operador de Canal

**Descrição:**
- Implemente as funcionalidades específicas para operadores de canal: KICK, INVITE, TOPIC, e MODE.
- Permita que operadores realizem essas ações e que as alterações sejam aplicadas corretamente aos canais.

**Tarefas:**
- Adicionar lógica para os comandos KICK (expulsar), INVITE (convidar), TOPIC (alterar tópico) e MODE (alterar modos do canal).
- Garantir que apenas operadores possam executar essas ações.

**Critérios de Aceitação:**
- Operadores devem conseguir usar os comandos KICK, INVITE, TOPIC e MODE com sucesso.
- As alterações feitas pelos operadores devem refletir corretamente no estado dos canais e usuários.

# Garantia de Robustez e Implementação de Testes

**Descrição:**
- Assegure que o servidor seja robusto e capaz de lidar com múltiplos clientes simultaneamente.
- Implementar testes para verificar a funcionalidade e a robustez do servidor, incluindo o manuseio de erros e situações de carga.

**Tarefas:**
- Implementar I/O não-bloqueante e utilizar `poll()` ou equivalente para todas as operações.
- Criar e executar testes para verificar o comportamento do servidor em condições de erro e carga.
- Documentar os procedimentos de teste e os resultados.

**Critérios de Aceitação:**
- O servidor deve operar sem travar e lidar eficientemente com múltiplos clientes.
- Todos os testes devem ser executados e passar, demonstrando a robustez e a funcionalidade correta do servidor.