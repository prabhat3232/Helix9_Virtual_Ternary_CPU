#include "neural_net.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>

// Helper: Save PPM Image
void SavePPM(const std::string& filename, const std::vector<TernaryFloat>& pixels, int width, int height) {
    std::ofstream file(filename);
    file << "P3\n" << width << " " << height << "\n255\n"; // RGB PPM header
    
    for(int i=0; i<width*height; ++i) {
        double val = pixels[i].ToDouble();
        // Clamp 0..1
        if (val < 0) val = 0;
        if (val > 1) val = 1;
        
        int c = (int)(val * 255.0);
        file << c << " " << c << " " << c << "\n"; // Grayscale
    }
    file.close();
}

// Generator: 200x200 Geometric Shapes
std::vector<TernaryFloat> GenerateShape(int type) {
    int W = 200;
    int H = 200;
    std::vector<TernaryFloat> img(W*H);
    // Init Black
    for(int i=0; i<W*H; ++i) img[i] = TernaryFloat(0,0);
    
    int cx = W/2;
    int cy = H/2;
    
    if (type == 0) { // Square (Centered 100x100)
        int size = 50;
        for(int y=cy-size; y<cy+size; ++y) {
            for(int x=cx-size; x<cx+size; ++x) {
                if(x>=0 && x<W && y>=0 && y<H)
                    img[y*W + x] = TernaryFloat(1, 0); 
            }
        }
    }
    else if (type == 1) { // Circle (Radius 50)
        int r = 50;
        for(int y=0; y<H; ++y) {
            for(int x=0; x<W; ++x) {
                if ((x-cx)*(x-cx) + (y-cy)*(y-cy) <= r*r) {
                    img[y*W + x] = TernaryFloat(1, 0);
                }
            }
        }
    }
    else if (type == 2) { // Cross
         int thick = 20;
         for(int y=0; y<H; ++y) {
             for(int x=0; x<W; ++x) {
                 if (abs(x-cx) < thick || abs(y-cy) < thick)
                    img[y*W + x] = TernaryFloat(1, 0);
             }
         }
    }
    return img;
}

int main() {
    std::srand(std::time(0));
    std::cout << "--- Helix-9 Generative AI (High-Res) ---" << std::endl;
    std::cout << "Target: 200x200 Images (40,000 Pixels)" << std::endl;
    
    // 1. Build Autoencoder
    // 40000 -> 256 (Sparse) -> 32 (Latent) -> 256 -> 40000
    TSAI_Model ae;
    ae.learning_rate = 0.05; 
    
    // Encode: 40k -> 256. Density 1% (400 connections per hidden neuron)
    ae.AddLayer(40000, 256, 0.01); 
    
    // Bottleneck: 256 -> 32. Density 100%
    ae.AddLayer(256, 32, 1.0);
    
    // Decode: 32 -> 256. Density 100%
    ae.AddLayer(32, 256, 1.0);
    
    // Output: 256 -> 40k. 
    // Increased Density to 25% (0.25) to fix "Starry Night" artifact.
    // Each pixel now sums input from ~64 neurons.
    ae.AddLayer(256, 40000, 0.25);
    
    std::cout << "Architecture: 40k -> 256(1%) -> 32 -> 256 -> 40k(25%)" << std::endl;
    
    // 2. Generate Dataset
    std::cout << "Generating Synthetic Dataset..." << std::endl;
    std::vector<std::vector<TernaryFloat>> dataset;
    for(int i=0; i<10; ++i) { // Reduced batch size for speed
        dataset.push_back(GenerateShape(i % 3)); 
    }
    
    // 3. Train (Reconstruction)
    int epochs = 50;
    std::cout << "Training for " << epochs << " epochs..." << std::endl;
    for(int epoch=0; epoch < epochs; ++epoch) {
        for(const auto& img : dataset) {
            ae.Train(img, img); 
        }
        ae.ApplyUpdates();
        
        if (epoch % 5 == 0) {
             std::cout << "Epoch " << epoch << " Complete." << std::endl;
        }
    }
    
    // 4. Save Reconstruction
    auto recon = ae.Forward(dataset[0]); // Square
    SavePPM("recon_square_hr.ppm", recon, 200, 200);
    std::cout << "Saved recon_square_hr.ppm" << std::endl;
    
    // 5. Dream!
    // Interpolation Dream
    auto imgA = dataset[0]; // Square
    auto imgB = dataset[1]; // Circle
    std::vector<TernaryFloat> hybrid(40000);
    for(int i=0; i<40000; ++i) {
         double val = (imgA[i].ToDouble() + imgB[i].ToDouble()) / 2.0;
         hybrid[i] = TernaryFloat::FromDouble(val);
    }
    
    auto dream_out = ae.Forward(hybrid);
    SavePPM("dream_hybrid_hr.ppm", dream_out, 200, 200);
    std::cout << "Saved dream_hybrid_hr.ppm" << std::endl;
    
    return 0;
}
