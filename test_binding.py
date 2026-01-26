import unittest
from helix9 import Helix9

class TestHelix9(unittest.TestCase):
    def setUp(self):
        try:
            self.helix = Helix9()
        except FileNotFoundError:
            self.skipTest("DLL not found - please build the project first")

    def test_cpu_memory(self):
        # Test Memory Read/Write
        addr = 100
        val = 123
        self.helix.write_mem(addr, val)
        read_val = self.helix.read_mem(addr)
        self.assertEqual(val, read_val, "Memory read should match write")

    def test_cpu_registers(self):
        # Test Register Access (Initial state should be 0)
        reg0 = self.helix.get_register(0)
        self.assertEqual(reg0, 0, "Register 0 should be initialized to 0")

    def test_ai_workflow(self):
        # Test basic AI setup and training
        self.helix.create_ai()
        self.helix.add_layer(2, 4, 1.0) # Input: 2, Hidden: 4
        self.helix.add_layer(4, 1, 1.0) # Hidden: 4, Output: 1
        self.helix.set_learning_rate(0.1)

        # Simple training step (dummy data)
        # Trying to teach it OR gate logic: 0,1 -> 1
        inputs = [0.0, 1.0]
        targets = [1.0]
        
        initial_loss = self.helix.train(inputs, targets)
        
        # Train a few times
        for _ in range(10):
            self.helix.train(inputs, targets)
            
        final_loss = self.helix.train(inputs, targets)
        
        # Loss should decrease (or at least not explode)
        print(f"Initial Loss: {initial_loss}, Final Loss: {final_loss}")
        self.assertLessEqual(final_loss, initial_loss, "Loss should decrease after training")

        # Prediction
        prediction = self.helix.predict(inputs, 1)
        self.assertEqual(len(prediction), 1)
        print(f"Prediction: {prediction[0]}")

if __name__ == '__main__':
    unittest.main()
