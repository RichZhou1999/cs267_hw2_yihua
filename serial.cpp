#include "common.h"
#include <cmath>
#include <set>
#include <unordered_map>
#include <iostream>

#define EPS  0.001

static std::unordered_map<int, std::set<int>> bins;
static int bin_size = cutoff;
static int lda;

// Apply the force from neighbor to particle
void apply_force(particle_t& particle, particle_t& neighbor) {
    // Calculate Distance
    double dx = neighbor.x - particle.x;
    double dy = neighbor.y - particle.y;
    double r2 = dx * dx + dy * dy;

    // Check if the two particles should interact
    if (r2 > cutoff * cutoff)
        return;

    r2 = fmax(r2, min_r * min_r);
    double r = sqrt(r2);

    // Very simple short-range repulsive force
    double coef = (1 - cutoff / r) / r2 / mass;
    particle.ax += coef * dx;
    particle.ay += coef * dy;
}

// Integrate the ODE
void move(particle_t& p, double size) {
    // Slightly simplified Velocity Verlet integration
    // Conserves energy better than explicit Euler method
    p.vx += p.ax * dt;
    p.vy += p.ay * dt;
    p.x += p.vx * dt;
    p.y += p.vy * dt;

    // Bounce from walls
    while (p.x < 0 || p.x > size) {
        p.x = p.x < 0 ? -p.x : 2 * size - p.x;
        p.vx = -p.vx;
    }

    while (p.y < 0 || p.y > size) {
        p.y = p.y < 0 ? -p.y : 2 * size - p.y;
        p.vy = -p.vy;
    }
}

int calculate_bin_number(double x, double y, double size, double bin_size, int lda){

    int k1=(int)x/bin_size;
    int column_index;
    if ( (x - k1*bin_size) < EPS){
        column_index = k1;
    }else{
        column_index = k1 + 1;
    }

    int k2=(int)y/bin_size;
    int row_index;
    if ((y-k2*bin_size) < EPS){
        row_index = k2;
    }else{
        row_index = k2 + 1;
    }
    return column_index + lda* row_index;

}


void init_simulation(particle_t* parts, int num_parts, double size) {
//    for( int i=0; i< lda*lda;i++){
//        bin[i] = std::set<int>;
//    }
    int index;
    for (int i = 0; i < num_parts; ++i){
        index = calculate_bin_number(parts[i].x,parts[i].y, size, bin_size,lda);
        bins[index].insert(i);
    }
	// You can use this space to initialize static, global data objects
    // that you may need. This function will be called once before the
    // algorithm begins. Do not do any particle simulation here
}

bool check_boundary(int row , int column){
    if ((row < 0) or (row>=lda)){
        return false;
    }
    if ((column < 0) or (column >=lda)){
        return false;
    }
    return true;
}
void apply_force_bin(int row, int column, int p, particle_t* parts){
    if (not check_boundary(row, column)){
        return;
    }
    for (auto it = bins[column+row*lda].begin(); it != bins[column+row*lda].end(); ++it){
        apply_force(parts[p], parts[*it]);
    }

}

void simulate_one_step(particle_t* parts, int num_parts, double size) {
    // Compute Forces
//    lda = (int) size/bin_size;
//    lda += 1;
    for (int i = 0; i < num_parts; ++i) {
        parts[i].ax = parts[i].ay = 0;
        for (int j = 0; j < num_parts; ++j) {
            apply_force(parts[i], parts[j]);
        }
    }

//    for(int i = 0; i < lda; ++i){
//        for (int j = 0; j < lda; ++j){
//            for (auto it = bins[i+j*lda].begin(); it != bins[i+j*lda].end(); ++it){
//                apply_force_bin(j-1,i-1, *it, parts);
//                apply_force_bin(j-1,i, *it, parts);
//                apply_force_bin(j-1,i+1, *it, parts);
//                apply_force_bin(j,i-1, *it, parts);
//                apply_force_bin(j,i+1, *it, parts);
//                apply_force_bin(j+1,i-1, *it, parts);
//                apply_force_bin(j+1,i, *it, parts);
//                apply_force_bin(j+1,i+1, *it, parts);
//                for (auto it2 = bins[i+j*lda].begin(); it2 != bins[i+j*lda].end(); ++it2){
//                    if(it2 != it){
//                        apply_force(parts[*it], parts[*it2]);
//                    }
//                }
//            }
//        }
//    }

    // Move Particles
    for (int i = 0; i < num_parts; ++i) {
        move(parts[i], size);
    }

//
}
