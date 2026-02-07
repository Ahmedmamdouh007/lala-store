import { useEffect } from 'react';
import './Toast.css';

const Toast = ({ message, type = 'success', onClose, duration = 2500 }) => {
  useEffect(() => {
    const t = setTimeout(() => onClose?.(), duration);
    return () => clearTimeout(t);
  }, [duration, onClose]);

  return (
    <div className={`toast toast-${type}`} role="alert">
      <span className="toast-icon">{type === 'success' ? '✓' : '!'}</span>
      <span>{message}</span>
    </div>
  );
};

export default Toast;
