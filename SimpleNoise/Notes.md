# SimpleNoise Notes

- Noise Level ramping

- Sample-wise VS Channel-wise



## Noise Level ramping

If we change Noise Level quickly, we will hear piercing "click" sound.  

For example, you suddenly turn the Noise Level up from 0.1 to 0.8 at a certain moment,
the audio sample value can undergo a huge step change within a sampling period. 

To prevent this annoying "click" sound, we usually perform a smoothing process.
This provides a smooth time period to transition the Noise Level between the two values.

```C++
// private:
float _currentLevel;
float _targetLevel;
int _samplesToTarget;
```

---

In this project, we use Linear Ramp.

> Calculate the difference between the current value and the target value,  
> and then distribute it equally among N sampling points.

```C++
auto levelIncrement = 
    (_targetLevel - _currentLevel) / static_cast<float>(_samplesToTarget);
```

---

Then use Sample-wise to traverse every sample.

```C++
for (sample){
    
    for (channel) {
        // generate noise
    }
    
    _currentLevel += levelIncrement;
    _samplesToTarget--;
}
```

We must update the gain **once per sample**, because the level changes continuously during the ramp.  

For stereo:
```txt
sample 0: L/R use same gain
sample 1: L/R use updated gain
sample 2: L/R use updated gain
```

This ensures both channels remain synchronized.


---

Process remaining samples


A single getNextAudioBlock() call processes one audio buffer.  

For example:
```txt
|--------------buffer size----------------|

|----ramp----|-----------stable-----------|
0           128                          512
```

After ramping finishes:

```C++
offset = numSamplesThisTime;
numSamplesRemaining -= numSamplesThisTime;
```
Now, the samples we need to smooth (_samplesToTarget or ramp) have been processed, then use fixed level to continue processing the remaining part (stable) of this buffer


--- 

## Sample-wise VS Channel-wise

The audio buffer is typically de-interleaved in memory.

For example:

```txt
[L,L,L,...][R,R,R,...]
```

---

### Channel-wise: 


- **Performance-oriented**
- **Chache Friendly**:  
The inner loop jumps continuously in memory, and CPU prefetching instructions can greatly improve efficiency.
- **SIMD**:  
The compiler can more easily optimize consecutive floating-point operations into Single Instruction Multiple Data (SIMD) instructions.

> Applicable scenarios: 
> - most DSP processing tasks (filtering, gain, convolution).

---

### Sample-wise:

- **logic-oriented**
- **Consistent Status**:   
Ensure that the L/R channels use the exact same _currentLevel at the same time.

> Applicable scenarios: 
> - gain smoothing (Ramping)
> - waveform synthesis (such as random noise)
> - dynamic processing of cross-channel correlation

