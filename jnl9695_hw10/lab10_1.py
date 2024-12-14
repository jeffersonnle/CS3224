import random
import plotly.graph_objects as go

def page_trace_generator(n, p):
    random.seed()  # Ensure reproducibility
    return [random.randint(0, p-1) for _ in range(n)]

def find_victim(page_table, frames, pointer):
    # Implementing the clock algorithm correctly with a pointer
    while True:
        page = frames[pointer]
        if page_table[page]['reference_bit'] == 0:
            return pointer
        page_table[page]['reference_bit'] = 0
        pointer = (pointer + 1) % len(frames)
    return pointer

def simulate(page_trace, f, p):
    frames = [] 
    page_table = {i: {'valid': False, 'reference_bit': 0} for i in range(p)}
    page_faults = 0
    pointer = 0  # Initialize pointer for the clock algorithm

    for page in page_trace:
        if not page_table[page]['valid']: 
            page_faults += 1
            if len(frames) < f:
                frames.append(page)
            else:
                victim_index = find_victim(page_table, frames, pointer)
                victim = frames[victim_index]
                page_table[victim] = {'valid': False, 'reference_bit': 0}
                frames[victim_index] = page
                pointer = (victim_index + 1) % f  # Move pointer forward
            page_table[page] = {'valid': True, 'reference_bit': 1}
        else:
            # This handles the case where the page is already in memory
            page_table[page]['reference_bit'] = 1

    return page_faults

def main():
    n = 64  
    p = 16  
    page_trace = page_trace_generator(n, p)

    results = []
    for f in range(4, p + 1):  
        faults = simulate(page_trace, f, p)
        results.append((f, faults))

    frames, faults = zip(*results)

    fig = go.Figure()
    fig.add_trace(go.Scatter(
        x=frames,
        y=faults,
        mode='lines+markers',
        name='Page Faults'
    ))

    fig.update_layout(
        title="Faults vs Frames Allocated",
        xaxis_title="Number of Frames Allocated",
        yaxis_title="Number of Faults",
        template="plotly_dark",
        showlegend=True
    )

    fig.show()

if __name__ == "__main__":
    main()
