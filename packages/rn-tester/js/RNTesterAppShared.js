import React, {useState} from 'react';
import {Button, SafeAreaView, ScrollView, StyleSheet, Switch, Text, TextInput} from 'react-native';

let logBuffer = [];

const log = (message) => {
    console.log(message);
    logBuffer.push(message);
};

const handleClearLog = () => {
    logBuffer = [];
};

function fibonacci(n) {
    if (n <= 0) {
        throw new Error('Invalid input. n must be a positive integer.');
    }

    if (n === 1 || n === 2) {
        return 1;
    }

    return fibonacci(n - 1) + fibonacci(n - 2);
}

const N = 35;

const App = () => {
    const textInputRef = React.useRef(null);
    const [text, setText] = useState('');
    const [shouldSlowDown, setShouldSlowDown] = useState(false);

    const handleClearInput = () => {
        log(`Clearing! Text: '${text}'. New text: ''.`);

        textInputRef.current.clear();
        setText('');
    };

    log(`Rendering TextInput with value = '${text}'`);

    if (shouldSlowDown) {
        log(`fib(N): ${fibonacci(N)}`);
    }

    return (
        <SafeAreaView style={styles.container}>
            <TextInput
                ref={textInputRef}
                style={styles.textInput}
                value={text}
                onChange={({nativeEvent}) => {
                    setText((previousText) => {
                        const {count, start, before, text: fullNewText, eventCount} = nativeEvent;
                        // This method is called to notify you that, within s, the count characters beginning at start have just
                        // replaced old text that had length before. It is an error to attempt to make changes to s from this callback.
                        const newText = fullNewText.substring(start, start + count);
                        // Replace newText in the original text:
                        const updatedText = previousText.substring(0, start) + newText + previousText.substring(start + before);

                        console.log('setting text state to:', updatedText, eventCount);
                        return updatedText;
                    });
                }}
                placeholder="Type something..."
            />

            <Text>Text: '{text}'</Text>

            <Button title="Clear input" onPress={handleClearInput}/>

            <Button title="Clear log" onPress={handleClearLog}/>

            <Text>Slow down:</Text>
            <Switch
                value={shouldSlowDown}
                onValueChange={(newValue) => {
                    log(`shouldSlowDown changed! oldValue: '${shouldSlowDown}'. newValue: '${newValue}'`);
                    setShouldSlowDown(newValue);
                }}
            />

            <ScrollView style={styles.logContainer}>
                {logBuffer.slice().reverse().map((message, index) => (
                    <Text key={index} style={styles.logText}>
                        {message}
                    </Text>
                ))}
            </ScrollView>
        </SafeAreaView>
    );
};

const styles = StyleSheet.create({
    container: {
        flex: 1,
        justifyContent: 'center',
        alignItems: 'center',
        padding: 16,
        margin: 16,
    },
    textInput: {
        width: '100%',
        height: 40,
        borderWidth: 1,
        borderColor: '#ccc',
        borderRadius: 4,
        paddingHorizontal: 8,
        marginBottom: 16,
    },
    button: {
        marginBottom: 16,
    },
    logContainer: {
        flex: 1,
        width: '100%',
        marginTop: 16,
        borderWidth: 1,
        borderColor: '#ccc',
        borderRadius: 4,
        padding: 8,
    },
    logText: {
        marginBottom: 4,
    },
});

export default App;
