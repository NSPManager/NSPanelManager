import React from 'react';
import styles from './styles.module.css';

const Centered = ({ children }) => {
    return (
        <div className={styles.centeredContent}>
            {children}
        </div>
    );
};

export default Centered;