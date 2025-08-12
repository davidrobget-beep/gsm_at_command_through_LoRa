#
class data_structure:
  def __init__(self, b_array, TX_Confirmation_waitingFor):
    self.b_array = b_array
    self.TX_Confirmation_waitingFor = TX_Confirmation_waitingFor

# * * * * * * * * * * *

class Node:
  def __init__(self, data_structure):
    self.data = data_structure
    self.next = None

class Queue:
  def __init__(self):
    self.front = None
    self.rear = None
    self.length = 0

  def enqueue(self, element):
    new_node = Node(element)
    if self.rear is None:
      self.front = self.rear = new_node
      self.length += 1
      return
    self.rear.next = new_node
    self.rear = new_node
    self.length += 1

  def isEmpty(self):
    return self.length == 0

  def size(self):
    return self.length

  def dequeue(self):
    if self.isEmpty():
      return None
    temp = self.front
    self.front = temp.next
    self.length -= 1
    if self.front is None:
      self.rear = None
    return temp.data

  def peek(self):
    if self.isEmpty():
      return None
    return self.front.data

  def isEmpty(self):
    return self.length == 0

  def clear(self):
    self.front = None
    self.rear = None
    self.length = 0

  # def printQueue(self):
  #   temp = self.front
  #   while temp:
  #     print(temp.data, end=" -> ")
  #     temp = temp.next
  #   print()

#comQueue = ComPortQueueM.Queue()

class anyConfirmations :
  def __init__(self):
    self.TX_ConfirmationGotten = 0    # original confirmation
    self.TX_Confirmation_waitingFor = 0


class Transmit_Manager:
  def __init__(self):
    self.queue = Queue()
    self.period_waitForConfirmation = 0
    self.attempts_to_send = 4
    self.CONST_ATTEMPTS_TO_SEND = 4
    self.CONST_PERIOD_WAIT_FOR_CONFIRMATION = 5000   # ms   4000 is in firmware now
    self.confirmations = anyConfirmations()
    self.gettingModemMessage = 0
    self.I_must_confirm = 0