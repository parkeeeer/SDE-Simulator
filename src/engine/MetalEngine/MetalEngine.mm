#include "MetalEngine.hpp"
#include <string>
#include <stdexcept>
#include <iostream>
#import <Metal/Metal.h>
#import <Foundation/Foundation.h>

template<sde::concepts::FloatingPoint Num>
sde::engine::GPU::MetalProgram<Num>::MetalProgram(const std::string& source){
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    if(!device){
        throw std::runtime_error("Metal Device could not be created.");
    }
    device_ = (__bridge_retained void*)device;

    id<MTLCommandQueue> queue = [device newCommandQueue];
    command_queue_ = (__bridge_retained void*)queue;

    NSString* src = [NSString stringWithUTF8String:source.c_str()];
    NSError* error = nil;

    id<MTLLibrary> library = [device newLibraryWithSource:src options:nil error:&error];
    if (!library) {
        NSLog(@"Metal compilation error: %@", error);
        throw std::runtime_error("Failed to compile Metal shader");
    }
    library_ = (__bridge_retained void*)library;

    id<MTLFunction> function = [library newFunctionWithName:@"sde_kernel"];
    if (!function) {
        throw std::runtime_error("Failed to find kernel function");
    }

    id<MTLComputePipelineState> pipeline = [device newComputePipelineStateWithFunction:function error:&error];
    if (!pipeline) {
        NSLog(@"Pipeline creation error: %@", error);
        throw std::runtime_error("Failed to create pipeline");
    }
    pipeline_ = (__bridge_retained void*)pipeline;
}

template<sde::concepts::FloatingPoint Num>
void sde::engine::GPU::MetalProgram<Num>::launch(Num* paths, uint64_t seed, Num dt, Num t0,
                               Num initial_value, unsigned int num_paths,
                               unsigned int num_steps) {
    id<MTLDevice> device = (__bridge id<MTLDevice>)device_;
    id<MTLCommandQueue> queue = (__bridge id<MTLCommandQueue>)command_queue_;
    id<MTLComputePipelineState> pipeline = (__bridge id<MTLComputePipelineState>)pipeline_;


    size_t paths_size = num_paths * num_steps * sizeof(Num);
    id<MTLBuffer> paths_buffer = [device newBufferWithBytes:paths
                                         length:paths_size
                                         options:MTLResourceStorageModeShared];


    id<MTLCommandBuffer> cmd_buffer = [queue commandBuffer];
    id<MTLComputeCommandEncoder> encoder = [cmd_buffer computeCommandEncoder];

    [encoder setComputePipelineState:pipeline];
    [encoder setBuffer:paths_buffer offset:0 atIndex:0];
    [encoder setBytes:&seed length:sizeof(uint64_t) atIndex:1];
    [encoder setBytes:&dt length:sizeof(Num) atIndex:2];
    [encoder setBytes:&t0 length:sizeof(Num) atIndex:3];
    [encoder setBytes:&initial_value length:sizeof(Num) atIndex:4];
    [encoder setBytes:&num_paths length:sizeof(unsigned int) atIndex:5];
    [encoder setBytes:&num_steps length:sizeof(unsigned int) atIndex:6];


    NSUInteger threadgroup_size = 256;
    NSUInteger num_threadgroups = (num_paths + threadgroup_size - 1) / threadgroup_size;

    [encoder dispatchThreadgroups:MTLSizeMake(num_threadgroups, 1, 1)
             threadsPerThreadgroup:MTLSizeMake(threadgroup_size, 1, 1)];

    [encoder endEncoding];
    [cmd_buffer commit];
    [cmd_buffer waitUntilCompleted];


    memcpy(paths, [paths_buffer contents], paths_size);
}

template<sde::concepts::FloatingPoint Num>
sde::engine::GPU::MetalProgram<Num>::~MetalProgram() {
    if (device_) CFRelease(device_);
    if (command_queue_) CFRelease(command_queue_);
    if (pipeline_) CFRelease(pipeline_);
    if (library_) CFRelease(library_);
}

template class sde::engine::GPU::MetalProgram<float>;
template class sde::engine::GPU::MetalProgram<double>;