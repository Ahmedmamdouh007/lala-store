import { Link } from 'react-router-dom';
import { useCart } from '../context/CartContext';
import './Navbar.css';

const Navbar = () => {
  const { getCartItemCount } = useCart();
  const cartCount = getCartItemCount();

  return (
    <nav className="navbar">
      <div className="navbar-container">
        <Link to="/" className="navbar-logo">
          LALA STORE
        </Link>
        <ul className="navbar-menu">
          <li>
            <Link to="/" className="navbar-link">
              Home
            </Link>
          </li>
          <li>
            <Link to="/men" className="navbar-link">
              Men
            </Link>
          </li>
          <li>
            <Link to="/women" className="navbar-link">
              Women
            </Link>
          </li>
          <li>
            <Link to="/cart" className="navbar-link cart-link">
              Cart ({cartCount})
            </Link>
          </li>
        </ul>
      </div>
    </nav>
  );
};

export default Navbar;
